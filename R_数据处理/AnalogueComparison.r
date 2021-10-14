####导入实验台数据采集器的监测数据####
# 此处可参考CommandAndControl

##数据导入的一些预处理
data.ep.raw<-fread(file="Nucleo测试_带平滑_20211013.csv")
data.ep.raw<-data.ep.raw[testId!=""&!is.na(testId)]

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


####IoT采集数据合并
data.ep.iotCollect.raw<-read.xlsx(file="../模拟输出稳定评估.xlsx",sheetName="实测_带10平滑")%>%as.data.table()#实验台实测_未调量程_8bit
data.ep.iotCollect.raw$time<-as.POSIXct(paste("2021-10-13",substr(as.character(data.ep.iotCollect.raw$time),1,8)))
data.ep.iotCollect.wide<-dcast(data = data.ep.iotCollect.raw,formula = No+time~variable+isSmooth)%>%as.data.table()


data.ep.iotCollect.wide$timeLabel<-as.character(data.ep.iotCollect.wide$time)

data.ep.autoCollect<-data.ep.test.second[,c("timeLabel","Time","Valveopening","Vset","Flowrate")]

#根据时间戳合并两者数据
data.ep.mappingOutput<-merge(data.ep.iotCollect.wide,data.ep.autoCollect,all.x = TRUE,by="timeLabel")
data.ep.mappingOutput$ID<-c(1:nrow(data.ep.mappingOutput))

data.ep.mappingOutput$FlowrateVotage_TRUE_Smooth<-getMovingAverageValue(data.ep.mappingOutput$FlowrateVotage_TRUE,5)

flowrateVar<-c("FlowrateCurrent_FALSE","FlowrateCurrent_TRUE","FlowrateVotage_TRUE_Smooth","FlowrateVotage_FALSE","FlowrateVotage_TRUE")
valveVar<-c("Valveopening_FALSE","Valveopening_TRUE")
data.ep.mappingOutput[,c("timeLabel","ID","Valveopening",..flowrateVar,..valveVar,"Flowrate")]%>%#"A2"
    melt(.,id.var=c("timeLabel","ID"))%>%{
        ggplot(.[variable=="Valveopening"],aes(x=ID,y=value,color=variable))+
            geom_path()+#geom_point()+
            geom_path(data=.[variable %in% c("Flowrate")],aes(x=ID,y=value*100))+
            geom_point(data=.[variable %in% c("Flowrate")],aes(x=ID,y=value*100))+
            geom_path(data=.[variable %in% c("FlowrateVotage_TRUE_Smooth","FlowrateVotage_TRUE")],aes(x=ID,y=100*(0.0024*value-2.3482)))+
            geom_path(data=.[variable %in% valveVar],aes(x=ID,y=(value*0.0428-35.67)))+
            # geom_path(data=.[variable %in% c("Output")],aes(x=ID,y=value/40.96))+
            theme_bw()+theme(axis.text=element_text(size=18),axis.title=element_text(size=18,face="bold"),#legend.position = c(0.25,0.75),
                             legend.text = element_text(size=16))#
    }

