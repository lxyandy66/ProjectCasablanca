####以下为原始直接从串口处读取数据处理，现已不再使用####
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
