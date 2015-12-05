library("RWeka")

trainSet<- read.csv("train.csv", header = TRUE)
testSet<- read.csv("test.csv", header = TRUE)
trainSet<- trainSet[, 2:602]

model <- PART(cuisine ~ ., data = trainSet, control = Weka_control(R= TRUE, M = 6, N =3))
predictedValue <- predict(model, testSet)
resultFrame <- data.frame(id=testSet[1], cuisine=predictedValue)
write.csv(resultFrame, file="result.csv", row.names=FALSE)

save(model, file = "rule-based_model.rda")
