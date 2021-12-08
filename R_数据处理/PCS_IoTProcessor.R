
data.iot.raw<-as.data.table(read.xlsx(file = "../IoTTest.xlsx",sheetName = "IoTtestV2F_1_20211206"))

#设置数据为毫秒级别
options(digits.secs=3)
data.iot.raw$datetime<-as.POSIXct(paste("2021-12-06 ",data.iot.raw$time))

data.iot.msg<-data.iot.raw[Msg=="Msg"]
data.iot.msg$receiveInv<-data.iot.msg%>%{.$datetime-c(NA,.[1:(nrow(.)-1)]$datetime)}%>%as.numeric(.)
data.iot.msg$jsonObj<-lapply(data.iot.msg$data,fromJSON)

data.iot.fmt<-dcast(data.iot.raw[,-"time"],formula = datetime+loopCount~Msg)
data.iot.fmt<-data.iot.raw[,.(
    recTime=min(datetime),
    actTime=max(datetime),
    reqId=as.numeric(as.character(value[Msg=="reqId"][1])),
    Qread=as.numeric(as.character(value[Msg=="FlowrateVotage"][1])),
    Qset=as.numeric(as.character(value[Msg=="Qset"][1])),
    ValveRead=as.numeric(as.character(value[Msg=="Valveopening"][1])),
    VsetIot=as.numeric(as.character(value[Msg=="Vset"][1])),
    msgLen=nchar(as.character(value[Msg=="Msg"][1]))
),by=loopCount]

data.iot.fmt[,-c("recTime","msgLen")]%>%melt(.,id.var=c("loopCount","actTime","reqId"))%>%{
    ggplot(data=.[!variable%in%c("Qread","Qset")],
           aes(x=loopCount,y=value,color=variable))+geom_line()+
        geom_line(data=.[variable%in%c("Qread","Qset")],
                  aes(x=loopCount,y=value*100,color=variable))
}

data.mas.first.raw$dataList<-lapply(data.mas.first.raw$data,fromJSON)


data.mas.first.raw<-data.mas.first.raw%>%mutate(.,resendTime=abs(.$resnd_time-.$recv_time),
                                                temp=extractFromList(.$dataList,"t"),
                                                lambda=extractFromList(.$dataList,"lm"),
                                                cv=extractFromList(.$dataList,"cv"))%>%as.data.table(.)
data.mas.first.raw[,c("data","dataList","cmd_type")]<-NULL

setorder(data.mas.first.raw,req_id,id)

#单步优化情况
ggplot(data.mas.first.raw[board_Type!="co"&req_id>15,c("req_id","board_id","board_Type","temp")]%>%
           mutate(.,req_id=.$req_id-30),
       aes(x=req_id,y=temp,color=board_id,group=board_id,shape=board_id))+geom_point()+geom_line()+
    theme_bw()+theme(axis.text=element_text(size=18),axis.title=element_text(size=18,face="bold"),
                     legend.text = element_text(size=16),legend.position = c(0.9,0.25))


#计算时间
ggplot(data.mas.first.raw[board_Type=="co"&comp_time>0,c("board_id","board_Type","comp_time")],#
       aes(x=board_id,y=comp_time,color=board_Type,group=board_id))+geom_boxplot(width=0.5)+
    theme_bw()+theme(axis.text=element_text(size=18),axis.title=element_text(size=18,face="bold"),
                     legend.text = element_text(size=16))

