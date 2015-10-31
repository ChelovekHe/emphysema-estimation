#ifndef __Hausdorff_h
#define __Hausdorff_h

/* 
   Let X,Y \subseteq M and d a metric on M.
   The Hausdorff distance is defined as
     d_H(X,Y) = max( sup_{x \in X} inf_{y \in Y} d(x,y), sup_{y \in Y} inf_{x \in X} d(x,y) )

 */
template<typename IterType, typename Distance>
typename Distance::ResultType
hausdorff(IterType xBegin, IterType xEnd, 
	  IterType yBegin, IterType yEnd,
	  size_t cols,
	  Distance dist) {
  
  typedef typename Distance::ResultType ResultType;

  // Calculate sup_{x \in X} inf_{y \in Y} d(x,y)
  ResultType sup_xy = std::numeric_limits<ResultType>::lowest();
  for ( auto x = xBegin; x < xEnd; x += cols ) {
    ResultType inf_xy = std::numeric_limits<ResultType>::max();
    for ( auto y = yBegin; y < yEnd; y += cols ) {
      auto distance = dist(x, y, cols);
      if ( distance < inf_xy ) {
	inf_xy = distance;
      }
    }
    if ( inf_xy > sup_xy) {
      sup_xy = inf_xy;
    }
  }

  // Calculate sup_{x \in X} inf_{y \in Y} d(x,y)
  ResultType sup_yx = std::numeric_limits<ResultType>::lowest();
  for ( auto y = yBegin; y < yEnd; y += cols ) {
    ResultType inf_yx = std::numeric_limits<ResultType>::max();
    for ( auto x = xBegin; x < xEnd; x += cols ) {
      auto distance = dist(x, y, cols);
      if ( distance < inf_yx ) {
	inf_yx = distance;
      }
    }
    if ( inf_yx > sup_yx) {
      sup_yx = inf_yx;
    }
  }
  
  return std::max(sup_xy, sup_yx);
}

#endif
