
library(openxlsx)
## Import data from original xlsx file...


d.dirty = read.xlsx("../lista_partecipanti.xlsx","FinalData",startRow = 6)

names(d.dirty)[2] <- "Group"
names(d.dirty)[8] <- "Treatment"
names(d.dirty)[9] <- "Outcome"
names(d.dirty)[13] <- "Elapsed"
names(d.dirty)[17] <- "Q3.SCORE"
names(d.dirty)[14:16] <- paste("C",1:3,sep=".")
names(d.dirty)[20:21] <- c("Q2","Q2.TOT")

d <- d.dirty[,c(1,2,8,9,13:21,27:28)]

save("d",file="CodeSplitData.R")


