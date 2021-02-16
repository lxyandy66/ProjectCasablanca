library(data.table)
library(rjson)
library(ggplot2)
library(dplyr)
data.mas.first.raw<-as.data.table(read.csv(file = "MAS_Command.csv"))%>%mutate(.,data=as.character(.$data))

data.mas.first.raw$dataList<-lapply(data.mas.first.raw$data,fromJSON)


data.mas.first.raw$temp<-data.mas.first.raw$dataList[["t"]]
data.mas.first.raw$lambda<-data.mas.first.raw$dataList[["lm"]]
data.mas.first.raw$cv<-data.mas.first.raw$dataList[["cv"]]
