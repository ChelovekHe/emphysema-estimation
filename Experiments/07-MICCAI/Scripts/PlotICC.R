#!/usr/bin/Rscript

main <- function(options) {
    num.options <- length( options );
    
    if ( num.options < 3 ) {
        print( "Usage: <icc-agreement> <icc-consistency> <out-prefix>" );
        quit( status = 1 );
    }
    plotICC(options[1], options[2], options[3])
    quit( status = 0 );
}

plotICC <- function( agreementPath, consistencyPath, outPrefix ) {
    regions <- c('ELL', 'ELM', 'ELU', 'ERL', 'ERM', 'ERU')
    
    ICC.agreement <- read.csv(agreementPath);
    ref <- ICC.agreement[ICC.agreement$Raters == 'L1.M1',]
    cms.m <- ICC.agreement[ICC.agreement$Raters == 'M1.CMS',]
    cms.l <- ICC.agreement[ICC.agreement$Raters == 'L1.CMS',]
    cms.lm <- ICC.agreement[ICC.agreement$Raters == 'L1.CMS',]
    
    pdf(file=paste(outPrefix,'ICC.agreement.pdf', sep=''))
    plot(x=c(),
         xlim=c(min(cms.m$k), max(cms.m$k)),
         ylim=c(-1,1),
         main="ICC agreement. Full line R1/R2, dashed R1/CMS, dotted R2/CMS",
         xlab="#Clusters",
         ylab="ICC agreement")
    i <- 1;
    for ( region in regions ) {
        lines(ref[ref$Region == region, c('k', 'ICC')], col=i, lty=1, lwd=3)
        lines(cms.l[cms.l$Region == region, c('k', 'ICC')], col=i, lty=2, lwd=3)
        lines(cms.m[cms.m$Region == region, c('k', 'ICC')], col=i, lty=3, lwd=3)
#        lines(cms.lm[cms.lm$Region == region, c('k', 'ICC')], col=i, lty=4, lwd=3)
        i <- i + 1
    }
    legend(x="bottom", legend=substr(regions,2,3), col=1:6, lwd=3)
    dev.off()


    ICC.consistency <- read.csv(consistencyPath)
    ref <- ICC.consistency[ICC.consistency$Raters == 'L1.M1',]
    cms.l1 <- ICC.consistency[ICC.consistency$Raters == 'L1.CMS',]
    cms.m <- ICC.consistency[ICC.consistency$Raters == 'M1.CMS',]
    cms.l <- ICC.consistency[ICC.consistency$Raters == 'L1.CMS',]

    pdf(file=paste(outPrefix, 'ICC.consistency.pdf', sep=''))
    plot(x=c(),
         xlim=c(min(cms.m$k), max(cms.m$k)),
         ylim=c(-1,1),
         main="ICC consistency. Full line R1/R2, dashed R1/CMS, dotted R2/CMS",
         xlab="#Clusters",
         ylab="ICC consistency")
    i <- 1;
    for ( region in regions ) {
        lines(ref[ref$Region == region, c('k', 'ICC')], col=i, lty=1, lwd=3)
        lines(cms.l[cms.l$Region == region, c('k', 'ICC')], col=i, lty=2, lwd=3)
        lines(cms.m[cms.m$Region == region, c('k', 'ICC')], col=i, lty=3, lwd=3)
        #lines(cms.lm[cms.lm$Region == region, c('k', 'ICC')], col=i, lty=4, lwd=3)
        i <- i + 1
    }
    legend(x="bottom", legend=substr(regions,2,3), col=1:6, lwd=3)
    dev.off()
}

options <- commandArgs(trailingOnly=TRUE);
main(options);
