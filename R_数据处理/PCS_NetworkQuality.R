###处理Carmen信号测试数据的脚本

data.carmen.indoor.raw<-read.xlsx(file="Carmen_IndoorSignal.xlsx",sheetIndex = 1)%>%as.data.table

data.carmen.indoor.raw.long<-melt(data.carmen.indoor.raw,id.var=c("packetId"))
data.carmen.indoor.raw.long[,":="(location=substring(variable,1,1),
                                  transmitter=substring(variable,2,2),
                                  destination=substring(variable,4))]
data.carmen.indoor.raw.long$value<-as.numeric(data.carmen.indoor.raw.long$value)

stat.carmen.indoor.raw<-data.carmen.indoor.raw.long[,.(location=location[1],
                                                       transmitter=transmitter[1],
                                                       destination=destination[1],
                                                       meanDelay=mean(value,na.rm = TRUE),
                                                       activeLoss=length(value[value>1000&!is.na(value)]),
                                                       networkedLoss=length(value[is.na(value)]),
                                                       availableDelay=mean(value[value<1000],na.rm=TRUE)
                                                       ),by=variable]

ggplot(data.carmen.indoor.raw.long,aes(x=destination,y=value,color=location))+
  geom_boxplot()+facet_wrap(.~transmitter)+ylim(c(0,1500))+stat_summary(fun.y = mean,geom="point")
