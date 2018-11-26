// Copyright 2002 Rensselaer Polytechnic Institute

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Lauren Foutz
//           Scott Hill

/*
  This file implements the functions

  template <class VertexListGraph, class DistanceMatrix, 
    class P, class T, class R>
  bool floyd_warshall_initialized_all_pairs_shortest_paths(
    const VertexListGraph& g, DistanceMatrix& d, 
    const bgl_named_params<P, T, R>& params)

  AND

  template <class VertexAndEdgeListGraph, class DistanceMatrix, 
    class P, class T, class R>
  bool floyd_warshall_all_pairs_shortest_paths(
    const VertexAndEdgeListGraph& g, DistanceMatrix& d, 
    const bgl_named_params<P, T, R>& params)
*/


#ifndef BOOST_GRAPH_FLOYD_WARSHALL_HPP
#define BOOST_GRAPH_FLOYD_WARSHALL_HPP

#include <boost/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/relax.hpp>
#include <algorithm> // for std::min and std::max

namespace boost
{
  namespace detail 
  {
  
    template<typename VertexListGraph, typename DistanceMatrix, typename BinaryPredicate, typename BinaryFunction, typename Infinity, typename Zero>
	bool floyd_warshall_dispatch(const VertexListGraph& g,  DistanceMatrix& d, const BinaryPredicate &compare, const BinaryFunction &combine, 
						const Infinity& inf, const Zero& zero)
    {
      BOOST_USING_STD_MIN();

      typename graph_traits<VertexListGraph>::vertex_iteratorN 
        i, lasti, j, lastj, k, lastk;
    
      /* main Floyd Warshall algorithm */
      for (tie(k, lastk) = vertices(g); k != lastk; k++)
        for (tie(i, lasti) = vertices(g); i != lasti; i++)
          for (tie(j, lastj) = vertices(g); j != lastj; j++)
          {
            d[*i][*j] = min BOOST_PREVENT_MACRO_SUBSTITUTION(d[*i][*j], combine(d[*i][*k], d[*k][*j]));
          }
      
    
		/* checks for negative weight cycle */
      for (tie(i, lasti) = vertices(g); i != lasti; i++)
      	if (compare(d[*i][*i], zero))
          return false;
      
	  return true;
    }
  

/* with predecessor map */
    template<typename VertexListGraph, typename DistanceMatrix, typename PredecessorMatrix, typename BinaryPredicate, typename BinaryFunction, typename Infinity, typename Zero>
	bool floyd_warshall_dispatch2(const VertexListGraph& g,  DistanceMatrix& d, PredecessorMatrix& p, const BinaryPredicate &compare, const BinaryFunction &combine, 
						const Infinity& inf, const Zero& zero)
    {
      BOOST_USING_STD_MIN();

      typename graph_traits<VertexListGraph>::vertex_iteratorN i, lasti, j, lastj, k, lastk;
    
      // main Floyd Warshall algorithm 
      for (tie(k, lastk) = vertices(g); k != lastk; k++)
        for (tie(i, lasti) = vertices(g); i != lasti; i++)
          for (tie(j, lastj) = vertices(g); j != lastj; j++)
          {
			if(d[*i][*j] > combine(d[*i][*k], d[*k][*j]))
				p[*i][*j] = p[*k][*j];

			d[*i][*j] = min BOOST_PREVENT_MACRO_SUBSTITUTION(d[*i][*j], combine(d[*i][*k], d[*k][*j]));
          }
      
    
		// checks for negative weight cycle 
      for (tie(i, lasti) = vertices(g); i != lasti; i++)
      	if (compare(d[*i][*i], zero))
          return false;
      
	  return true;
    }
  }

  template <typename VertexListGraph, typename DistanceMatrix, typename BinaryPredicate, typename BinaryFunction,typename Infinity, typename Zero>
  bool floyd_warshall_initialized_all_pairs_shortest_paths(const VertexListGraph& g, DistanceMatrix& d, const BinaryPredicate& compare, 
				    const BinaryFunction& combine, const Infinity& inf, const Zero& zero)
  {
    function_requires<VertexListGraphConcept<VertexListGraph> >();
  
    return detail::floyd_warshall_dispatch(g, d, compare, combine, inf, zero);
  }
  

  
  template <typename VertexAndEdgeListGraph, typename DistanceMatrix, typename WeightMap, typename BinaryPredicate, typename BinaryFunction, typename Infinity, typename Zero>
  bool floyd_warshall_all_pairs_shortest_paths( const VertexAndEdgeListGraph& g, DistanceMatrix& d, const WeightMap& w, 
  					 const BinaryPredicate& compare, const BinaryFunction& combine, const Infinity& inf, const Zero& zero)
  {
    BOOST_USING_STD_MIN();

    function_requires<VertexListGraphConcept<VertexAndEdgeListGraph> >();
    function_requires<EdgeListGraphConcept<VertexAndEdgeListGraph> >();
    function_requires<IncidenceGraphConcept<VertexAndEdgeListGraph> >();
  
    typename graph_traits<VertexAndEdgeListGraph>::vertex_iteratorN 	firstv, lastv, firstv2, lastv2;
    typename graph_traits<VertexAndEdgeListGraph>::edge_iteratorN 	first, last;
  

    for(tie(firstv, lastv) = vertices(g); firstv != lastv; firstv++)
      for(tie(firstv2, lastv2) = vertices(g); firstv2 != lastv2; firstv2++)
	    d[*firstv][*firstv2] = inf;


    for(tie(firstv, lastv) = vertices(g); firstv != lastv; firstv++)
      d[*firstv][*firstv] = 0;
    

    for(tie(first, last) = edges(g); first != last; first++)
    {
      if (d[source(*first, g)][target(*first, g)] != inf)
        d[source(*first, g)][target(*first, g)] = min BOOST_PREVENT_MACRO_SUBSTITUTION(get(w, *first), d[source(*first, g)][target(*first, g)]);
      else 
        d[source(*first, g)][target(*first, g)] = get(w, *first);	
    }
    
    bool is_undirected = is_same<typename graph_traits<VertexAndEdgeListGraph>::directed_category, undirected_tag>::value;
    if (is_undirected)
    {
      for(tie(first, last) = edges(g); first != last; first++)
      {
        if (d[target(*first, g)][source(*first, g)] != inf)
          d[target(*first, g)][source(*first, g)] = min BOOST_PREVENT_MACRO_SUBSTITUTION(get(w, *first), d[target(*first, g)][source(*first, g)]);
        else 
          d[target(*first, g)][source(*first, g)] = get(w, *first);
      }
    }
    
  
    return detail::floyd_warshall_dispatch(g, d, compare, combine, inf, zero);
  }

/* with predecessor map */
  template <typename VertexAndEdgeListGraph, typename DistanceMatrix, typename PredecessorMatrix, typename WeightMap, typename BinaryPredicate, typename BinaryFunction, typename Infinity, typename Zero>
  bool floyd_warshall_all_pairs_shortest_paths2( const VertexAndEdgeListGraph& g, DistanceMatrix& d, PredecessorMatrix& p, const WeightMap& w, 
  					 const BinaryPredicate& compare, const BinaryFunction& combine, const Infinity& inf, const Zero& zero)
  {
    BOOST_USING_STD_MIN();

    function_requires<VertexListGraphConcept<VertexAndEdgeListGraph> >();
    function_requires<EdgeListGraphConcept<VertexAndEdgeListGraph> >();
    function_requires<IncidenceGraphConcept<VertexAndEdgeListGraph> >();
  
    typename graph_traits<VertexAndEdgeListGraph>::vertex_iteratorN 	firstv, lastv, firstv2, lastv2;
    typename graph_traits<VertexAndEdgeListGraph>::edge_iteratorN 	first, last;
  
    // initialize matrix: distance infinity
    for(tie(firstv, lastv) = vertices(g); firstv != lastv; firstv++)
      for(tie(firstv2, lastv2) = vertices(g); firstv2 != lastv2; firstv2++)
      { 
	  	d[*firstv][*firstv2] = inf;
		p[*firstv][*firstv2] = *firstv;
		//cerr << "[FW::Initialization]  p: " << *firstv << " = " << p[*firstv][*firstv2] << endl;
	  }
    
    // initialize matrix: distance to itself i zero
    for(tie(firstv, lastv) = vertices(g); firstv != lastv; firstv++)
      d[*firstv][*firstv] = 0;
    

    for(tie(first, last) = edges(g); first != last; first++)
    {
      if (d[source(*first, g)][target(*first, g)] != inf)
	  {
	  	cerr << "[Floyd-Warshall] ERROR Predecessor map " << endl;
        d[source(*first, g)][target(*first, g)] = min BOOST_PREVENT_MACRO_SUBSTITUTION(get(w, *first), d[source(*first, g)][target(*first, g)]);
	  }
      else 
	  {
        d[source(*first, g)][target(*first, g)] = get(w, *first);	
		p[source(*first, g)][target(*first, g)] = source(*first, g); // enter predecessor map here
	    //cerr << "[FW::Initialization] (" << source(*first, g) << "," << target(*first, g) << ") Parent map: " << p[source(*first, g)][target(*first, g)];
		//cerr << " Weight Map: " << d[source(*first, g)][target(*first, g)] << endl;
	  }
    }
    
    bool is_undirected = is_same<typename graph_traits<VertexAndEdgeListGraph>::directed_category, undirected_tag>::value;
    if (is_undirected)
    {
      for(tie(first, last) = edges(g); first != last; first++)
      {
        if (d[target(*first, g)][source(*first, g)] != inf)
		{
		  cerr << "[Floyd-Warshall] Undirected: ERROR Predecessor map " << endl;
          d[target(*first, g)][source(*first, g)] = min BOOST_PREVENT_MACRO_SUBSTITUTION(get(w, *first), d[target(*first, g)][source(*first, g)]);
		}
        else 
		{
          d[target(*first, g)][source(*first, g)] = get(w, *first);
		  p[target(*first, g)][source(*first, g)] = target(*first, g); // enter predecessor map here
		  //cerr << "[FW::Initialization] (" << target(*first, g) << "," << source(*first, g) << ") UndParent map: " << p[target(*first, g)][source(*first, g)];
		  //cerr << " Weight Map: " << d[target(*first, g)][source(*first, g)] << endl;
		}
      }
    }
    
  
    return detail::floyd_warshall_dispatch2(g, d, p, compare, combine, inf, zero);
  }


  namespace detail {        
    template <class VertexListGraph, class DistanceMatrix, class WeightMap, class P, class T, class R>
    bool floyd_warshall_init_dispatch(const VertexListGraph& g, DistanceMatrix& d, WeightMap w, const bgl_named_params<P, T, R>& params)
    {
      typedef typename property_traits<WeightMap>::value_type WM;
    
      return floyd_warshall_initialized_all_pairs_shortest_paths(g, d,
        choose_param(get_param(params, distance_compare_t()), 
          std::less<WM>()),
        choose_param(get_param(params, distance_combine_t()), 
          closed_plus<WM>()),
        choose_param(get_param(params, distance_inf_t()), 
          std::numeric_limits<WM>::max BOOST_PREVENT_MACRO_SUBSTITUTION()),
        choose_param(get_param(params, distance_zero_t()), 
          WM()));
    }
    

    
    template <class VertexAndEdgeListGraph, class DistanceMatrix, class WeightMap, class P, class T, class R>
    bool floyd_warshall_noninit_dispatch(const VertexAndEdgeListGraph& g, DistanceMatrix& d, WeightMap w, const bgl_named_params<P, T, R>& params)
    {
      typedef typename property_traits<WeightMap>::value_type WM;
    
      return floyd_warshall_all_pairs_shortest_paths(g, d, w,
        choose_param(get_param(params, distance_compare_t()), 
          std::less<WM>()),
        choose_param(get_param(params, distance_combine_t()), 
          closed_plus<WM>()),
        choose_param(get_param(params, distance_inf_t()), 
          std::numeric_limits<WM>::max BOOST_PREVENT_MACRO_SUBSTITUTION()),
        choose_param(get_param(params, distance_zero_t()), 
          WM()));
    }


/* With predecessor map */
    template <class VertexAndEdgeListGraph, class DistanceMatrix, class PredecessorMatrix, class WeightMap, class P, class T, class R>
    bool floyd_warshall_noninit_dispatch2(const VertexAndEdgeListGraph& g, DistanceMatrix& d, PredecessorMatrix& p, WeightMap w, const bgl_named_params<P, T, R>& params)
    {
      typedef typename property_traits<WeightMap>::value_type WM;
    
      return floyd_warshall_all_pairs_shortest_paths2(g, d, p, w,
        choose_param(get_param(params, distance_compare_t()), 
          std::less<WM>()),
        choose_param(get_param(params, distance_combine_t()), 
          closed_plus<WM>()),
        choose_param(get_param(params, distance_inf_t()), 
          std::numeric_limits<WM>::max BOOST_PREVENT_MACRO_SUBSTITUTION()),
        choose_param(get_param(params, distance_zero_t()), 
          WM()));
    }

    

  }   // namespace detail

  
/* VertexListGraphs:   */  
  template <class VertexListGraph, class DistanceMatrix, class P, class T, class R>
  bool floyd_warshall_initialized_all_pairs_shortest_paths(const VertexListGraph& g, DistanceMatrix& d, const bgl_named_params<P, T, R>& params)
  {
    return detail::floyd_warshall_init_dispatch(g, d, choose_const_pmap(get_param(params, edge_weight), g, edge_weight), params);
  }
  
  template <class VertexListGraph, class DistanceMatrix>
  bool floyd_warshall_initialized_all_pairs_shortest_paths(const VertexListGraph& g, DistanceMatrix& d)
  {
    bgl_named_params<int,int> params(0);
    return detail::floyd_warshall_init_dispatch(g, d, get(edge_weight, g), params);
  }
  

/* VertexAndEdgeListGraphs:   */
  template <class VertexAndEdgeListGraph, class DistanceMatrix, class P, class T, class R>
  bool floyd_warshall_all_pairs_shortest_paths(const VertexAndEdgeListGraph& g, DistanceMatrix& d, const bgl_named_params<P, T, R>& params)
  {
    return detail::floyd_warshall_noninit_dispatch(g, d, choose_const_pmap(get_param(params, edge_weight), g, edge_weight), params);
  }
  
  template <class VertexAndEdgeListGraph, class DistanceMatrix>
  bool floyd_warshall_all_pairs_shortest_paths(const VertexAndEdgeListGraph& g, DistanceMatrix& d)
  {
    bgl_named_params<int,int> params(0);
    return detail::floyd_warshall_noninit_dispatch(g, d, get(edge_weight, g), params);
  }

/* With predecessor map */
  template <class VertexAndEdgeListGraph, class DistanceMatrix, class PredecessorMatrix, class P, class T, class R>
  bool floyd_warshall_all_pairs_shortest_paths2(const VertexAndEdgeListGraph& g, DistanceMatrix& d, PredecessorMatrix& p, const bgl_named_params<P, T, R>& params)
  {
    return detail::floyd_warshall_noninit_dispatch2(g, d, p, choose_const_pmap(get_param(params, edge_weight), g, edge_weight), params);
    //return detail::floyd_warshall_noninit_dispatch(g, d, choose_const_pmap(get_param(params, edge_weight), g, edge_weight), params);
  }
  

} // namespace boost

#endif

