KL <- function(p, q) {
  res <- 0
  for (i in 1:length(p)) {
    res <- res + p[i] * log2(p[i]/q[i])
  }
  res
}
A <- c(1/2, 1/2)
B <- c(1/4, 3/4)
C <- c(1/8, 7/8)
KL(A,B)
KL(B,A)
KL(A,C)
KL(C,A)
KL(B,C)
KL(C,B)