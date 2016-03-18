#!/usr/bin/Rscript

main <- function(options) {
    num.options <- length( options );
    
    if ( num.options < 2 ) {
        print( "Usage: <kappa> <out-path>" );
        quit( status = 1 );
    }
    plotKappa(options[1], options[2])
    quit( status = 0 );
}

plotKappa <- function( kappaPath, outpath ) {
    regions <- c('ELL', 'ELM', 'ELU', 'ERL', 'ERM', 'ERU')
    
    kappa <- read.csv(kappaPath);
    ref <- kappa[kappa$Raters == 'L1.M1',]
    cms.m <- kappa[kappa$Raters == 'M1.CMS',]
    cms.l <- kappa[kappa$Raters == 'L1.CMS',]
    cms.lm <- kappa[kappa$Raters == 'L1.CMS',]
    
    pdf(file=outpath)
    plot(x=c(),
         xlim=c(min(cms.m$k), max(cms.m$k)),
         ylim=c(-1,1),
         main="kappa agreement. Full line L1/M1, dashed L1/CMS, dotted M1/CMS",
         xlab="#Clusters",
         ylab="kappa")
    i <- 1;
    for ( region in regions ) {
        lines(ref[ref$Region == region, c('k', 'kappa')], col=i, lty=1, lwd=3)
        lines(cms.l[cms.l$Region == region, c('k', 'kappa')], col=i, lty=2, lwd=3)
        lines(cms.m[cms.m$Region == region, c('k', 'kappa')], col=i, lty=3, lwd=3)
        lines(cms.lm[cms.lm$Region == region, c('k', 'kappa')], col=i, lty=4, lwd=3)
        i <- i + 1
    }
    legend(x="bottom", legend=regions, col=1:6, lwd=3)
    dev.off()
}

options <- commandArgs(trailingOnly=TRUE);
main(options);
