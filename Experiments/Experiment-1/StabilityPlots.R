stability.1 <- read.csv("Stability-1.out");
postscript(file="Stability-1-boxplot.ps",
           bg="white"
           );
boxplot(stability.1[stability.1[,1] == 2, 3],
        stability.1[stability.1[,1] == 4, 3],
        stability.1[stability.1[,1] == 6, 3],
        stability.1[stability.1[,1] == 8, 3],
        stability.1[stability.1[,1] == 10, 3],
        stability.1[stability.1[,1] == 16, 3],
        stability.1[stability.1[,1] == 32, 3],
        stability.1[stability.1[,1] == 64, 3],
        names=c("2","4","6","8","16","32","64"),
        xlab="k",
        ylab="Hausdorff distance",
        main="Distribution of Hausdorff distance between cluster centers"
        );
dev.off();

# Do the distribution of random centers
random.centers <- read.csv("RandomCentersDistance.out");
postscript(file="RandomCentersDistance-boxplot.ps",
           bg="white"
           );
boxplot(random.centers[random.centers[,1] == 2, 2],
        random.centers[random.centers[,1] == 4, 2],
        random.centers[random.centers[,1] == 6, 2],
        random.centers[random.centers[,1] == 8, 2],
        random.centers[random.centers[,1] == 10, 2],
        random.centers[random.centers[,1] == 16, 2],
        random.centers[random.centers[,1] == 32, 2],
        random.centers[random.centers[,1] == 64, 2],
        names=c("2","4","6","8","16","32","64"),
        xlab="k",
        ylab="Hausdorff distance",
        main="Distribution of Hausdorff distance between random centers"
        );
dev.off();
