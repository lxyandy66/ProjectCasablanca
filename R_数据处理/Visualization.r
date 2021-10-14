
data.mas.first.raw<-as.data.table(read.csv(file = "MAS_CT不同参数.csv"))%>%mutate(.,data=as.character(.$data))

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

