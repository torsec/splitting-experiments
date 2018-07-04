# Table: data.txt
# Reports: R1, R2, R3, R4, R5, R6
# Annotations: a1, a2, a3, a4, a5
# Partitions: C = {R1, R2}, T = {R3, R4}, TS = {R5, R6}

library(textir)
threshold <- 0
d <- read.table(file = "data.txt")
t <- tfidf(d)
f <- function(x) {return (x > threshold)}
for (i in 1:nrow(t)) {
	print(rownames(t)[i])
	print(Filter(f, sort(t[i,], decreasing=TRUE)))
}
d <- read.table(file = "partitioned-data.txt")
t <- tfidf(d)
f <- function(x) {return (x > threshold)}
for (i in 1:nrow(t)) {
	print(rownames(t)[i])
	print(Filter(f, sort(t[i,], decreasing=TRUE)))
}
