types_algos <- c("basique", "opti")
premiers_max <-  c(5*10^5, 10^6, 2*10^6, 4*10^6)
data <- data.frame(x = seq(0,7))

for (type in types_algos) {
  for (nmax in premiers_max) {
    syncro <- read.csv(sprintf("./data/%d/%s/syncro.csv",nmax,type))$data
    thread_1 <- read.csv(sprintf("./data/%d/%s/thread_1.csv",nmax,type))$data
    thread_2 <- read.csv(sprintf("./data/%d/%s/thread_2.csv",nmax,type))$data
    thread_3 <- read.csv(sprintf("./data/%d/%s/thread_3.csv",nmax,type))$data
    thread_4 <- read.csv(sprintf("./data/%d/%s/thread_4.csv",nmax,type))$data
    thread_5 <- read.csv(sprintf("./data/%d/%s/thread_5.csv",nmax,type))$data
    thread_6 <- read.csv(sprintf("./data/%d/%s/thread_6.csv",nmax,type))$data
    thread_7 <- read.csv(sprintf("./data/%d/%s/thread_7.csv",nmax,type))$data
    
    avg_threads <- c(mean(syncro), mean(thread_1), mean(thread_2), mean(thread_3),
                     mean(thread_4), mean(thread_5), mean(thread_6), mean(thread_7))
    
    data[sprintf("%s_%d", type, nmax)] <- avg_threads
  }
}

color=c("red", "blue", "green", "purple")


plot(data$x, data$basique_4000000, type="b", col=color[4], lwd=2, pch=15,
     xlab="Nb thread (0 = pas multithreadé)", ylab="Temps d'execution (en ms)",
     ylim=range(0, max(data$basique_4000000)))
k <- 3
for (type in types_algos) {
  for (i in seq(1, k)) {
    var <- sprintf("%s_%d", type, premiers_max[i])
    lines(data$x, data[[var]], 
          type="b", col=color[i], lwd=2, pch= if (type == "opti") 19 else 15)
  }
  k <- k + 1
}
