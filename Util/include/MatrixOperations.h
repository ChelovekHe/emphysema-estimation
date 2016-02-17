#ifndef __MatrixOperations_h
#define __MatrixOperations_h


/**
   Make a co-occurence matrix of the values in the sequences [begin1,end1) and
   [begin2,end2). 
   If the sequences are not of the same length, then extra values in the longer
   range are ignored.

   out will contain the counts of co-occuring value, such that if value
   i and value j occur together n times, then out(i,j) == n.

   [begin1,end1) should contain integral values in [0, rows in out)
   [begin2,end2) should contain integral values in [0, cols in out)

   out should be initialied by the caller.

   @param begin1    iterator to begining of sequence1
   @param end1      iterator to end of sequence1
   @param begin2    iterator to begining of sequence2
   @param end2      iterator to end of sequence2
   @param out       Matrix object that support operator()(i,j). Elements in out
                    should support operator++. 
 */
template< typename TMatrix,
	  typename InputIter1,
	  typename InputIter2 >
TMatrix&
coOccurenceMatrix(InputIter1 begin1,
		  const InputIter1 end1,
		  InputIter2 begin2,
		  const InputIter2 end2,
		  TMatrix& out ) {
  while ( begin1 < end1 && begin2 < end2 ) {
    ++out(*begin1++, *begin2++);
  }
  return out;
}


/**
   Normalize so each row sum to one
*/
template< typename TEigenMatrix >
TEigenMatrix&
rowNormalize( TEigenMatrix& M ) {
  M = M.cwiseQuotient( M.rowwise().sum().rowwise().replicate( M.cols() ) );
  return M;
}

#endif
