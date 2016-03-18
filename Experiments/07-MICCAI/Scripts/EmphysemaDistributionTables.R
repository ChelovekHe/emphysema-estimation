#!/usr/bin/Rscript

main <- function(options) {
    usage <- "Usage: <visual-scores> <outpath>"
    if ( length(options) < 2 ) {
        cat( usage, '\n' );
        quit( status = 1 );
    }
        
    DistributionTable( options[1], options[2] )
    quit( status = 0 );
}

DistributionTable <- function(visualScoresPath, outpath) {
    library(lattice)
    scores <- read.csv(visualScoresPath, header=TRUE);

    regions <- c("ERU", "ERM", "ERL", "ELU", "ELM", "ELL");
    group.names <- c()
    
    classes <- c(1,2,3,4,5,6);
    scores[scores == 0] <- 1;
    counts <- c()
    names <- c()
    
    for ( region in regions ) {
        L1 <- scores[, paste(region, 'L1', sep='.')]
        M1 <- scores[, paste(region, 'M1', sep='.')]
        L1M1 <- pmax(L1,M1)

        counts <- cbind(counts,
                        CalculateCounts( L1, classes ),
                        CalculateCounts( M1, classes ),
                        CalculateCounts( L1M1, classes ));
        names <- cbind(names,
                       paste(substr(region, 2, 3), 'R1'),
                       paste(substr(region, 2, 3), 'R2'),
                       paste(substr(region, 2, 3), 'max'))
    }
    print( counts )
    colnames( counts ) <- names
    rownames( counts ) <- c("0%", "1-5%", "6-25%", "26-50%", "51-75%", "76-100%")
    pdf( file=outpath )
    props <- prop.table( counts, 2)
    print( round(props,3) )
    bar.colors <- heat.colors(nrow(props))
    par(mar=c(5.1, 4.1, 4.1, 7.1), xpd=TRUE)
    barplot(props,
            col=bar.colors,
            names.arg=names,
            las=2,
            space=c(
                0,
                0.2, 0.2, 0.75,
                0.2, 0.2, 0.75,
                0.2, 0.2, 0.75,
                0.2, 0.2, 0.75,
                0.2, 0.2, 0.75,
                0.2, 0.2),
            main="Distribution of emphysema extent in regions"
            )
    legend(x='topright',
           rownames( counts ),
           fill=bar.colors,
           inset=c(-0.25,0),
           title="Extent"
           )
    dev.off()
}


CalculateCounts <- function(x, classes) {
    counts <- rep(0, length(classes))
    for ( i in 1:length(classes) ) {
        counts[i] <- sum(x == classes[i]);
    }
    counts
}

CalculateFreq <- function(x, classes) {
    counts <- CalculateCounts(x, classes) / length(x)
}


options <- commandArgs(trailingOnly=TRUE);
main(options);
