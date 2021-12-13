
data.iot.raw<-as.data.table(read.xlsx(file = "IoTTest.xlsx",sheetName = "IoT_PoorNet_MV=1_BV_0"))

#设置数据为毫秒级别
options(digits.secs=3)
data.iot.raw$datetime<-as.POSIXct(paste("2021-12-09 ",data.iot.raw$time))
data.iot.raw$var<-as.character(data.iot.raw$var)

data.iot.msg<-data.iot.raw[var=="Msg"]
data.iot.msg$recInv<-data.iot.msg%>%{.$datetime-c(NA,.[1:(nrow(.)-1)]$datetime)}%>%as.numeric(.)
data.iot.msg$jsonObj<-lapply(as.character(data.iot.msg$value),fromJSON)
data.iot.msg$testIotId<-"Poor"

data.iot.fmt<-dcast(data.iot.raw[,-"time"],formula = datetime+loop~var,fun.aggregate = paste,collapse="")
#注意，这里一个loop不同的时间会出现两条，即一个loop下，Msg时间一条，记录参数一条，不影响


data.iot.fmt<-data.iot.raw[,.(
    iotTestId="Poor",
    recTime=min(datetime[var=="Msg"],na.rm = TRUE),
    actTime=max(datetime[var!="Msg"],na.rm = TRUE),
    reqId=as.numeric(as.character(value[var=="reqId"][1])),
    Qread=as.numeric(as.character(value[var=="FlowrateVotage"][1])),
    Qset=as.numeric(as.character(value[var=="Qset"][1])),
    ValveRead=as.numeric(as.character(value[var=="Valveopening"][1])),
    VsetIot=as.numeric(as.character(value[var=="Vset"][1])),
    msgLen=nchar(as.character(value[var=="Msg"][1])),
    msgCount=length(value[var=="Msg"])
),by=loop]
data.iot.fmt$timeLabel<-format(data.iot.fmt$actTime,format="%Y-%m-%d %H:%M:%S")
data.iot.fmt$msgNormal<-1
data.iot.fmt[is.na(msgCount)|is.na(msgLen)|msgCount<1|msgLen>50]$msgNormal<-0

data.iot.fmt[,-c("recTime","msgLen","msgLen")]%>%melt(.,id.var=c("loop","actTime","reqId","msgCount"))%>%{
    ggplot(data=.[!variable%in%c("Qread","Qset")],
           aes(x=loop,y=value,color=variable))+geom_line()+geom_line(aes(x=loop,y=msgCount*10))+
        geom_line(data=.[variable%in%c("Qread","Qset")],
                  aes(x=loop,y=value*100,color=variable))
}
# rm(data.iot.serLog.combine,data.iot.msg.combine)
temp.names<-names(data.iot.serLog.combine)
# data.ep.combine.temp[,..temp.names]
data.iot.serLog.combine<-rbind(data.iot.serLog.combine,data.iot.fmt)
data.iot.msg.combine<-rbind(data.iot.msg.combine,data.iot.msg)

# data.iot.serLog.combine[data.iot.serLog.combine$labelTime%in% duplicated(labelTime)]%>%View

####机柜数据导入及处理####
data.ep.raw<-fread(file="IoT_PoorNet_MV=1_BV=0_20211209.csv")
data.ep.raw<-data.ep.raw[testId %in% c("IoT_1210_Ideal_MV=1_BV=0_4","IoT_1210_Weak_MV=1_BV=0")]


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

####根据时间戳合并两者数据####
data.ep.test.second$revTimeCount<-data.ep.test.second$timeCount+5

data.ep.combine.temp<-merge(data.ep.test.second,data.iot.serLog.combine,
                       all.x = TRUE,by.x="timeLabel")


# data.ep.combine<-rbind(data.ep.combine,data.ep.combine.temp)

#可视化
manualViewCol<-c("timeCount","testId","Flowrate","InWindT","t_out_set","flow_set","ValveRead","VsetIot")
data.ep.combine[testId=="IoT_PoorNet_MV=1_BV=0",#&timeCount>300&timeCount<400
                    c("timeCount","testId","ValveRead","VsetIot","Flowrate",
                      "InWindT","t_out_set","flow_set","Qset","Qread","msgNormal"#,"predPressureStatus","resistanceS","isOn"
                    )]%>%#,,"testVset","testVset",,"Kp","Ti","flow_set"
    melt(.,id.var=c("timeCount","testId"))%>%#,"Ti","para","testVset"
    as.data.table(.)%>%{ #,"InWindT","t_out_set","resistanceS"
        ggplot(data=.[variable %in% c("ValveRead","VsetIot")],aes(x=timeCount,y=value,color=variable,width=4,group=paste(testId,variable)))+
            # geom_line()+#geom_point()+
            # geom_point(data=.[variable %in% c("Subpressure")],aes(x=timeCount,y=value))+
            geom_line(data=.[variable %in% c("Flowrate")],aes(x=timeCount,y=value*100))+#)+value#"OutWindT",(value-20)*5)
            geom_line(data=.[variable %in% c("Qread")],aes(x=timeCount,y=value*100,lty=variable))+# geom_line(data=.[variable %in% c("msgCount")],aes(x=timeCount,y=value*100))+
            # geom_line(data=.[variable %in% c("msgNormal")],aes(x=timeCount,y=value*100))+
            # geom_line(data=.[variable %in% c("InWindT","t_out_set")],aes(x=timeCount,y=value,lty=variable))+
            # geom_line(data=.[variable %in% c("OutWindT","t_return_set")],aes(x=timeCount,y=value))+
            scale_y_continuous(sec.axis = sec_axis(~./100,name = "Flow rate"))+#./5+20
             # xlim(c(0,750))+
            facet_wrap(~testId,ncol = 1)+
            theme_bw()+theme(axis.text=element_text(size=18),axis.title=element_text(size=18,face="bold"),#legend.position = c(0.25,0.75),
                             legend.text = element_text(size=16))#
    }

data.ep.combine[testId=="IoT_1210_Weak_MV=1_BV=0"&t_out_set==45&timeCount>20][,.(
    duration=min(timeCount,na.rm = TRUE),
    peakime=timeCount[InWindT==min(InWindT,na.rm = TRUE)][1],
    startTout=mean(InWindT[timeCount%in%1:10],na.rm=TRUE),#开始时刻的送风温度
    startTroom=mean(OutWindT[timeCount%in%1:10],na.rm=TRUE),#开始时刻的室内温度
    startFlowrate=mean(Flowrate[timeCount%in%1:10],na.rm=TRUE),#开始时刻的流量
    meanValveBias=mean(abs(Valveopening-Vset),na.rm=TRUE),
    meanLastTout=mean(InWindT[timeCount%in% timeCount[(length(timeCount)-60):length(timeCount)]],na.rm = TRUE),#最后100s的波动情况#(length(timeCount)-60:length(timeCount))
    sdLastTout=sd(InWindT[timeCount%in% timeCount[(length(timeCount)-60):length(timeCount)]],na.rm = TRUE),#最后100s的波动情况
    maxTout=min(InWindT,na.rm = TRUE),
    overshoot=(min(InWindT,na.rm = TRUE)/mean(InWindT[timeCount%in% c((length(timeCount)-60):length(timeCount))],na.rm = TRUE))-1
),by=testId]

