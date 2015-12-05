# create a variable called junk, 1=greek 0=non-greek
data$junk <- rep(0,nrow(data))
data$junk[which(data$cuisine %in% "greek")] <- 1
data$junk <- as.factor(data$junk)

datagreek <- data[,-c(1,2)]

# generate logistic regression
glm.fit = glm(junk~., data=datagreek, family = binomial)
summary(glm.fit)

library(MASS)
# prune model
stepAIC(glm.fit)
