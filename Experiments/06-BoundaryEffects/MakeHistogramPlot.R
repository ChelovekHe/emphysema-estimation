vol32 <- read.csv("~/emphysema-estimation/Experiments/05-LLP/Data/Bags/Training_RightUpper/vol32_20051129-D-1.dcm.bag", header=FALSE);
i1 <- as.numeric(vol32[1,]);
blur <- i1[1:41];
gradient <- i1[42:82];
eig1 <- i1[83:123]
eig2 <- i1[124:164]
eig3 <- i1[165:205]
LoG <- i1[206:246]
curvature <- i1[247:287]
frobenius <- i1[288:328]



png(file='Histograms/vol32_gaussian_blur.png')
barplot(ylim=c(0,0.06), blur, main="[32] Gaussian blur [< -945: > -552]")
dev.off()

png(file='Histograms/vol32_gradient.png')
barplot(ylim=c(0,0.06), gradient, main="[32] Gradient")
dev.off()

png(file='Histograms/vol32_eig1.png')
barplot(ylim=c(0,0.06), eig1, main="[32] Eigenvalue 1")
dev.off()

png(file='Histograms/vol32_eig2.png')
barplot(ylim=c(0,0.06), eig2, main="[32] Eigenvalue 2 [< -48: > 29]")
dev.off()

png(file='Histograms/vol32_eig3.png')
barplot(ylim=c(0,0.06), eig3, main="[32] Eigenvalue 3 [< -12: > 14]")
dev.off()

png(file='Histograms/vol32_log.png')
barplot(ylim=c(0,0.06), LoG, main="[32] Laplacian of Gaussian")
dev.off()

png(file='Histograms/vol32_curvature.png')
barplot(ylim=c(0,0.06), curvature, main="[32] Gaussian curvature")
dev.off()

png(file='Histograms/vol32_frobenius.png')
barplot(ylim=c(0,0.06), frobenius, main="[32] Frobenius norm [< 5: > 120]")
dev.off()


vol3151 <- read.csv("~/emphysema-estimation/Experiments/05-LLP/Data/Bags/Training_RightUpper/vol3151_20051011-D-1.dcm.bag", header=FALSE);
i1 <- as.numeric(vol3151[1,]);
blur <- i1[1:41];
gradient <- i1[42:82];
eig1 <- i1[83:123]
eig2 <- i1[124:164]
eig3 <- i1[165:205]
LoG <- i1[206:246]
curvature <- i1[247:287]
frobenius <- i1[288:328]

png(file='Histograms/vol3151_gaussian_blur.png')
barplot(ylim=c(0,0.06), blur, main="[3151] Gaussian blur[< -945: > -552] ")
dev.off()

png(file='Histograms/vol3151_gradient.png')
barplot(ylim=c(0,0.06), gradient, main="[3151] Gradient")
dev.off()

png(file='Histograms/vol3151_eig1.png')
barplot(ylim=c(0,0.06), eig1, main="[3151] Eigenvalue 1")
dev.off()

png(file='Histograms/vol3151_eig2.png')
barplot(ylim=c(0,0.06), eig2, main="[3151] Eigenvalue 2 [< -48: > 29]")
dev.off()

png(file='Histograms/vol3151_eig3.png')
barplot(ylim=c(0,0.06), eig3, main="[3151] Eigenvalue 3 [< -12: > 14]")
dev.off()

png(file='Histograms/vol3151_log.png')
barplot(ylim=c(0,0.06), LoG, main="[3151] Laplacian of Gaussian")
dev.off()

png(file='Histograms/vol3151_curvature.png')
barplot(ylim=c(0,0.06), curvature, main="[3151] Gaussian curvature")
dev.off()

png(file='Histograms/vol3151_frobenius.png')
barplot(ylim=c(0,0.06), frobenius, main="[3151] Frobenius norm [< 5: > 120]")
dev.off()
