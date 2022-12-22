####20211228更新，串口数据直接从数据库中读取，并标准化为JSON格式####
##IoT数据
#读取数据及格式调整
data.iot.raw<-read.csv(file = "./NCS_Data/IoT_SpeedTestAndMapping_1229.csv",stringsAsFactors=FALSE)%>%as.data.table()
# data.iot.raw<-fread(file = "./NCS_Data/IoT_20220422_new.csv")
options(digits.secs = 3)
data.iot.raw$msg_logTime<-as.POSIXct(data.iot.raw$msg_logTime)
unique(data.iot.raw$msg_testId)
# validTestId<-c("IoT_1230_Ideal_MV=1_BV=0","IoT_1230_Well_MV=1_BV=0_5","IoT_1230_Weak_MV=1_BV=0_3","Conv_1230_MV=1_BV=0_5")
data.iot.raw<-data.iot.raw[!is.na(msg_testId)&msg_testId!=""]#&msg_testId%in%validTestId
# data.iot.raw<-data.iot.raw[!is.na(msg_testId)&msg_testId=="SpeedTest"]

data.iot.raw<-data.iot.raw[startsWith(msg_content,"{")]

#串口信息的预处理
# 删去提示性信息
data.iot.cmd<-data.iot.raw[nchar(msg_content)<40]#删掉一些串口提示信息
data.iot.raw<-data.iot.raw[nchar(msg_content)>40]#删掉一些串口提示信息,含Ture接管的指令，这一部分理论应该在JSON中处理，但是包调用有问题


####JSON数据取出####
#注意，有些时候JSON里面有两个双引号
data.iot.raw[,msg_content:=gsub('""','"',msg_content)]

data.iot.raw$msgJson<-lapply(data.iot.raw$msg_content,FUN = jsonToListProcessor)


# rjson::fromJSON("{\"cmd\":\"CT_SP\",\"id\":\"C_FR\",\"rq\":3,\"sp\":\"0.000\"}{\"cmd\":\"CT_STS\"}",unexpected.escape = "keep") #不知道怎么回事就是弄不了
nameFromJson<-c("lp","rq","id","Qs","Qr","Vs","Vr","cmd","id" ,"rq" ,"sp" )
data.iot.raw[,':='(reqId=extractFromList(msgJson,"rq"),loop=extractFromList(msgJson,"lp"),
                   Qs=extractFromList(msgJson,"Qs"),Qr=extractFromList(msgJson,"Qr"),
                   # Ts=extractFromList(msgJson,"Ts"),Tsr=extractFromList(msgJson,"Tsr"),#取决于跨网络的数据传输
                   Vs=extractFromList(msgJson,"Vs"),Vr=extractFromList(msgJson,"Vr"),
                   sp=extractFromList(msgJson,"sp"),cmd=extractFromList(msgJson,"cmd"),
                   val=extractFromList(msgJson,"val"))]
data.iot.raw[,':='(sp=as.numeric(sp),val=as.numeric(val))]

####实际控制器执行的数据####
#如果只考虑实际执行，则只取串口的输出数据，即为用于采样的数据
data.iot.raw.exc<-data.iot.raw[msg_testId=="IoT_V2O_MV=1_BV=0.25_LTE2"&
                                   msg_source=="Serial"&!is.na(loop),-c("msg_id","msg_source","msg_content","msgJson","sp","cmd","val")]
data.iot.raw.exc<-data.iot.raw.exc[!duplicated(data.iot.raw.exc[,"msg_label"])]


####数据整理和延迟计算####
# 注意，一个msg_label可能对应多个rq，且持久化的代码可能有点问题，存在资源竞争
# reqId和msg_label不一定相等，一般source为UDP（即电脑端）持久化的大部分相等

setorder(data.iot.raw,msg_logTime)
data.iot.stat<-data.iot.raw[is.na(loop),#不考虑IoT自身状态回显的内容
                 .(count=length(msg_logTime),
                   msg_testId=msg_testId[1],
                   msg_label=msg_label[msg_source=="UDP"][1],
                   sndTime=msg_logTime[msg_source=="SerialSend"][1],
                   rcvTime=msg_logTime[msg_source=="Serial"&!is.na(cmd)][1],
                   rcvCount=length(msg_logTime[msg_source=="Serial"]),
                   sndCount=length(msg_logTime[msg_source=="SerialSend"])#,#UDP
                   # Ts=mean(Ts,na.rm=TRUE),
                   # Tsr=mean(Tsr,na.rm=TRUE),
                   # Vs=mean(Vs,na.rm=TRUE),
                   # Vr=mean(Vr,na.rm=TRUE),
                   # sp=mean(sp,na.rm=TRUE),
                   # val=mean(val,na.rm=TRUE)
                   ),by=msg_content]

data.iot.stat[,delay:=rcvTime-sndTime]
ggplot(data=data.iot.stat,aes(x=delay))+geom_density()

nrow(data.iot.stat[delay>1])/nrow(data.iot.stat[!is.na(rcvTime)])#active packet loss，即延迟大于1秒
nrow(data.iot.stat[!is.na(rcvTime)])/nrow(data.iot.stat)#纯丢包

mean(data.iot.stat[delay<0.005]$delay,na.rm=TRUE)
sd(data.iot.stat[delay<0.005]$delay,na.rm=TRUE)

write.xlsx(data.iot.stat,file="PacketAnalysis_SpeedTest.xlsx")

#数据整理
#串口回送的UDP数据，数据源为串口且cmd字段不为空
data.iot.fmt<-data.iot.raw[,.(
    sendTime=msg_logTime[msg_source=="UDP"][1],
    rcvTime=msg_logTime[msg_source=="Serial"&!is.na(cmd)][1],
    rqSend=max(reqId[msg_source=="UDP"][1],na.rm = TRUE),#PC端发送的UDP包中reqId
    rqRecv=max(reqId[msg_source=="Serial"&!is.na(cmd)][1],na.rm = TRUE),#本地收到回送的reqId
    rqLocal=max(reqId[msg_source=="Serial"&is.na(cmd)][1],na.rm = TRUE),#本地控制用的reqId
    localMsgCount=length(msg_logTime[msg_source=="Serial"]),
    netMsgCount=length(msg_logTime[msg_source=="UDP"]),
    localNetMsgCount=length(msg_logTime[msg_source=="Serial"&!is.na(cmd)]),
    loopCount=length(unique(loop[!is.na(loop)])),
    Qs=mean(Qs,na.rm=TRUE),
    Qr=mean(Qr,na.rm=TRUE),
    Vs=mean(Vs,na.rm=TRUE),
    Vr=mean(Vr,na.rm=TRUE),
    sp=mean(sp,na.rm=TRUE),
    val=mean(val,na.rm=TRUE)
    ),by=paste(msg_label,msg_testId)]

##计算接收消息和发送消息的间隔
#去掉一些异常值
data.iot.fmt[rqLocal<0]$rqLocal<-NA
ggplot(data.iot.fmt,aes(x=rqLocal))+geom_density()

data.iot.fmt[,":="(timeInv=rcvTime-sendTime,rqRcvInv=rqSend-rqRecv,rqActInv=rqSend-rqLocal)]#,isNormal:=ifelse(rqRcvInv==0,1,0)
data.iot.fmt[,isNormal:=ifelse(rqRcvInv==0,1,0)]

data.iot.fmt[,c("msg_label","rqActInv","rqRcvInv")]%>%melt(.,id.var="msg_label")%>%ggplot(.,aes(x=variable,y=value))+geom_boxplot()

mean(data.iot.fmt[msg_label<900&!is.infinite(rqRcvInv)]$rqRcvInv,na.rm = TRUE)
mean(data.iot.fmt[msg_label<900&!is.infinite(rqActInv)]$rqActInv,na.rm = TRUE)

#统计一下非正常的情况
# 一个Label（PC端循环）肯定是一个UDP
# 但一个Label内可能有多个串口消息
# 正常为2，即1UDP转发，1状态输出，但由于时间错开

#有效利用率
table(data.iot.fmt[msg_label<500&msg_label>300]$localNetMsgCoun)
data.iot.fmt[msg_label<900]%>%mutate(.,first=msg_label>750)%>%as.data.table(.)%>%{
    cat(mean(.[first==TRUE]$timeInv,na.rm=TRUE),mean(.[first==FALSE]$timeInv,na.rm=TRUE))
    ggplot(data = .,aes(x=as.numeric(timeInv*1000-12.95)))+geom_density()+facet_wrap(.~first,nrow=2)+
        theme_bw()+theme(axis.text=element_text(size=18),axis.title=element_text(size=18,face="bold"),#legend.position = c(0.25,0.75),
                         legend.text = element_text(size=16))#
}

#计算实际收到的包数，即包含乱序/应用层重叠接收/延迟的情况
# 实际上现在按照井号分割之后就不能用这个方法了
nchar(data.iot.raw[msg_testId=="IoT_0701_V2O_MV=1_BV=0.25_LTE"&msg_source=="Serial"&!is.na(cmd)&msg_label<900]$msg_content)%>%{ceiling(./49)}%>%sum()

ggplot(data=data.iot.fmt,aes(x=as.numeric(timeInv)*1000))+geom_density()

#应用的包数 包括粘滞的
#似乎从raw里面也直接看更能看出来
nrow(data.iot.raw[msg_testId=="IoT_V2O_MV=1_BV=0.25_LTE2"&msg_source=="Serial"&is.na(loop)]) #
nrow(data.iot.raw[msg_testId=="IoT_V2O_MV=1_BV=0.25_LTE2"&msg_source=="UDP"])
data.iot.raw[msg_testId=="IoT_V2O_MV=1_BV=0.25_LTE2",c("msg_logTime","msg_label","msg_content")]%>%View()

#按机柜一个label下，收到了多少个Serial来的包可以看出来粘滞情况
# 即一秒钟，可能由于粘滞，和井号切割 收到多个UDP包/缓存的UDP包，在串口中迅速读取
table(data.iot.raw[msg_testId=="IoT_V2O_MV=1_BV=0.25_LTE2"&msg_source=="Serial",c("msg_logTime","msg_label","msg_content")]$msg_label)%>%View

# save(data.iot.combine,data.iot.msg.combine,data.ep.combine,file = "TCP_旧版串口复制数据.rdata")

####机柜数据导入及处理####
data.ep.raw<-fread(file="./NCS_Data/Exp_20220628_LTE.csv")
data.ep.raw<-data.ep.raw[!is.na(testId)&testId!=""]


#不能直接label，因为每次重新执行labView时会重置，因此可能重复
data.ep.raw$Time<-as.POSIXct(data.ep.raw$Time)#注意如果之前用excel基本处理之后，时间格式可能不能统一
data.ep.raw$timeLabel<-format(data.ep.raw$Time,format="%Y-%m-%d %H:%M:%S")
####整合至秒级####
setorder(data.ep.raw,Time)
data.ep.test.second<-cbind(data.ep.raw[,.(Time=Time[1],ID=ID[1],Label=Label[1],
                                          testId=getMode(testId,na.rm = TRUE)[1],
                                          Vset=getMode(Vset,na.rm = TRUE)[1]),by=timeLabel],
                           data.ep.raw[,lapply(.SD,mean,na.rm=TRUE),
                                       .SDcols=c("Flowrate","Totalpressure","Subpressure","InWaterT","OutWaterT",
                                                 "InWindT","OutWindT","Valveopening","Fset","Tset",  #"Vset"  ,   
                                                 "t_out_set","t_return_set","flow_set",
                                                 "Powerset","Fre","HeatingRate"),by=timeLabel][,-"timeLabel"])%>%
    mutate(.,Time=as.POSIXct(.$Time))%>%as.data.table()

data.ep.test.second$timeCount<- -999
for(i in unique(data.ep.test.second[testId!=""&!is.na(testId)]$testId)){
    data.ep.test.second[testId==i]$timeCount<-0:(nrow(data.ep.test.second[testId==i])-1)
}

data.ep.test.second<-data.ep.test.second[testId=="IoT_V2O_MV=1_BV=0.25_LTE2"]


####根据Label和msg_label合并两者数据####
data.ep.combine<-merge(x=data.ep.test.second[testId=="IoT_V2O_MV=1_BV=0.25_LTE2"],#&Time>as.POSIXct("2022-04-16 17:40")
                       y=data.iot.raw.exc[msg_testId=="IoT_V2O_MV=1_BV=0.25_LTE2"],
                       all.x=TRUE,by.x = "Label",by.y="msg_label")

#Mapping校正用
ggplot(data.ep.combine,aes(y=Vr,x=Valveopening))+geom_point()
lm(Vr~Valveopening,data.ep.combine)%>%summary()#[Flowrate>0.1]
write.xlsx(data.ep.combine[,c("Time","loopCount","Label","Flowrate","Qr","Valveopening","Vr")],file = "自动采集_映射校正_1229.xlsx")


#统计
data.ep.combine[testId=="IoT_0701_V2O_MV=1_BV=0.25_LTE",c("Label","timeCount","reqId","InWindT","Tsr")]%>%View


#可视化
#"Conv_V2O_MV=1_BV=0.25"
manualViewCol<-c("Time","timeCount","testId","Flowrate","InWindT","t_out_set","flow_set","Vr","Vs","sp","isNormal","Vset","Valveopening","Tsr")#
data.ep.combine[testId=="IoT_V2O_MV=1_BV=0.25_LTE2"][,#&timeCount>300&timeCount<400timeCount%in%c(625:750)
                    c("timeCount","testId","Flowrate","Tsr",#"Vr","Vs","Tsr","Ts",#,"Valveopening"
                      "InWindT","t_out_set","flow_set"#,"Qs","Qr"#,"isNormal"#,"predPressureStatus","resistanceS","isOn"
                    )]%>%#,,"testVset","testVset",,"Kp","Ti","flow_set"
    melt(.,id.var=c("timeCount","testId"))%>%#,"Ti","para","testVset"
    as.data.table(.)%>%{ #,"InWindT","t_out_set","resistanceS"
        ggplot(data=.[!variable %in% c("Qr","Qs","flow_set","Flowrate")],
               aes(x=timeCount,y=value,color=variable,width=4,group=paste(testId,variable)))+#variable %in% c("Vr","Vs")
            geom_line()+#geom_point()+
            # geom_point(data=.[variable %in% c("Subpressure")],aes(x=timeCount,y=value))+
            geom_line(data=.[variable %in% c("flow_set","Qs")],aes(x=timeCount,y=value*125))+#)+value#"OutWindT",(value-20)*5),"sp","flow_set","Flowrate",
            geom_line(data=.[variable %in% c("Flowrate","Qr")],aes(x=timeCount,y=value*125,lty="twodash"))+# geom_line(data=.[variable %in% c("msgCount")],aes(x=timeCount,y=value*100))+
            # geom_line(data=.[variable %in% c("isNormal")],aes(x=timeCount,y=value*100))+
            # geom_line(data=.[variable %in% c("InWindT","t_out_set")],aes(x=timeCount,y=value,lty=variable))+
            # geom_line(data=.[variable %in% c("OutWindT","t_return_set")],aes(x=timeCount,y=value))+
            scale_y_continuous(sec.axis = sec_axis(~./125,name = "Flow rate"))+#./5+20
             # xlim(c(0,750))+
            facet_wrap(~testId,ncol = 1)+
            theme_bw()+theme(axis.text=element_text(size=18),axis.title=element_text(size=18,face="bold"),#legend.position = c(0.25,0.75),
                             legend.text = element_text(size=16))#
    }



data.ep.combine[testId=="IoT_V2O_MV=1_BV=0.25_LTE2"][t_out_set==43][,.(#data.ep.combine.weak[t_out_set==45]
    startTime=min(timeCount,na.rm = TRUE),
    peakMaxTime=timeCount[InWindT==max(InWindT,na.rm = TRUE)][1],
    peakMinTime=timeCount[InWindT==min(InWindT,na.rm = TRUE)][1],
    startTout=mean(InWindT[timeCount%in%1:10],na.rm=TRUE),#开始时刻的送风温度
    # startTroom=mean(OutWindT[timeCount%in%1:10],na.rm=TRUE),#开始时刻的室内温度
    startFlowrate=mean(Flowrate[timeCount%in%1:10],na.rm=TRUE),#开始时刻的流量
    meanValveBias=mean(abs(Valveopening-Vset),na.rm=TRUE),
    meanLastTout=mean(InWindT[timeCount%in% timeCount[(length(timeCount)-60):length(timeCount)]],na.rm = TRUE),#最后100s的波动情况#(length(timeCount)-60:length(timeCount))
    sdLastTout=sd(InWindT[timeCount%in% timeCount[(length(timeCount)-60):length(timeCount)]],na.rm = TRUE),#最后100s的波动情况
    #注意阶跃方向
    maxTout=max(InWindT,na.rm = TRUE),
    minTout=min(InWindT,na.rm = TRUE),
    #注意overshoot的min或者max
    overshootHigh=(max(InWindT,na.rm = TRUE)/mean(InWindT[timeCount%in% c((length(timeCount)-60):length(timeCount))],na.rm = TRUE))-1,
    overshootLow=(min(InWindT,na.rm = TRUE)/mean(InWindT[timeCount%in% c((length(timeCount)-60):length(timeCount))],na.rm = TRUE))-1
),by=testId]

