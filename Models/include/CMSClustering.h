#ifndef __CMSClustering_h
#define __CMSClustering_h

/**
   A set of cluster centers learned from bagged instances with a mapping from
   clusters to bags.
 */
template< typename MatrixType >
class CMSClustering {
public:
  CMSClustering() : m_Centroids(), m_C() {}
  CMSClustering( const MatrixType& centroids, const MatrixType& C )
    : m_Centroids( centroids ),
      m_C( C )
  {}

  std::size_t K() const {
    return Centroids.rows();
  }

  const MatrixType& Centroids() { return m_Centroids; }
  const MatrixType& C() { return m_C; }

private:
  MatrixType m_Centroids, m_C;
};
