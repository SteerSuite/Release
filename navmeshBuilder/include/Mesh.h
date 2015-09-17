//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/* Copyright (c) Darcy Harisson, Russell Gillette
 * April 2014
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * Edited by Glen Berseth
 *
 */
#ifndef MESH_MESH_H_
#define MESH_MESH_H_


#include "util/Geometry.h"

#include <cstdlib>
#include <vector>
#include "UtilGlobals.h"
//#define USE_PREV

const std::size_t HOLE_INDEX = static_cast<std::size_t>( -1 );

struct half_edge{
	std::size_t next; // Index of next half-edge in the loop.
#ifdef USE_PREV
	std::size_t prev; // Index of the previous half-edge in the loop. Alternatively: use next->next if you have strictly triangle meshes.
#endif
	std::size_t twin; // Index of half-edge that is in other face that shares this edge. Open edges?
	std::size_t vert; // Index of vertex at the start of this edge.
	std::size_t face; // Index of face to the "left" of this edge.
};

class Mesh;

/* an iterator over the one ring of vertices pointed to by this
 * half edge */

class vvert_iterator{
public:
	friend class Mesh;
	const half_edge* m_cur;
	const half_edge* m_end;
};

class vface_iterator{
public:
	friend class Mesh;
	const half_edge* m_cur;
	const half_edge* m_end;
	const half_edge* m_next;
};

class fvert_iterator{
public:
	friend class Mesh;
	const half_edge* m_cur;
	const half_edge* m_end;
};

class fface_iterator{
public:
	friend class Mesh;
	const half_edge* m_cur;
	const half_edge* m_end;
};

struct curvature_data
{
	double min_curvature;
	double max_curvature;
	double average_curvature;
	std::vector<double> curvature;
	std::vector<double> acceleration;
	double max_acceleration_allowed;

};

class UTIL_API Mesh{
public:
	Mesh();

	/**
	 * Initialize the mesh from existing data.
	 * \param xyzPositions A list of doubles, storing the vertex data interleaved (ie. X1,Y1,Z1,X2,Y2,Z2,etc.)
	 * \param triangleVerts A list of vertex indices, where each run of 3 defines a triangle in the mesh. (ie. T1.v1, T1.v2, T1.v3, T2.v1, T2.v2, T2.v3, etc.)
	 */
	void init( const std::vector<double>& xyzPositions, const std::vector<std::size_t>& triangleVerts );
	void init( const std::vector<Util::Point>& Positions, const std::vector<std::size_t>& triangleVerts );

	std::size_t add_vertex( double x, double y, double z );
	std::size_t add_vertex( Util::Point v );
	std::size_t add_face( std::size_t v1, std::size_t v2, std::size_t v3 );
	std::size_t add_face( std::size_t (&f)[3] );
	
	void delete_face( std::size_t f );
	std::size_t collapse_edge( std::size_t v1, std::size_t v2 );
	
	std::size_t split_face_center( std::size_t f, std::size_t (*pOutFaceIndices)[3] = NULL );

    /**
     * returns the position of a vertex
     * \param i the vertex index (not the he index)
     * \return Vector3d the vertex position values
     */
	Util::Point get_vertex( std::size_t i ) const;
	Util::Vector get_vnormal( std::size_t i ) const;
	Util::Vector get_fnormal( std::size_t i ) const;

	void set_vertex( std::size_t i, const Util::Point& v );
	size_t splitHalfEdgeProper(size_t he);
	virtual size_t closestVert(Util::Point p);
	

	size_t drawState;

	/*
	 * Utils for ray intersection
	 */
	bool ray_intersect_triangle (const Util::Point &ray_origin, const Util::Point &point_on_ray, Util::Point triangle_verts[3]);
	bool ray_intersect_triangle (const Util::Point &ray_origin, const Util::Point &point_on_ray, size_t tri);

    /*================================================
     * Iterator functions
     *================================================*/
public:
	/**
	 * Initialize an iterator that visits the 1-ring of 'vertex'.
	 * \param it The iterator to initialize.
	 * \param vertex The index of the vertex to iterate around.
	 * \return False if the vertex has no neighbors (ie. a floating vertex)
	 */
	bool init_iterator( vvert_iterator& it, std::size_t vertex ) const;
	
    /**
     * move the iterator to a boundary, if one exists, and set its end
     * to the current location
     * \param it The iterator to move
     * \return False if there is no boundary
     */
    bool reset_boundary_iterator( vvert_iterator& it ) const;

    /**
     * \param face index
     * \return true if on boundary false else
     */
    bool isBoundaryFace( std::size_t i ) const;

	/**
	 * Advances a vertex iterator to the next vertex in the 1-ring.
	 * \param it The iterator to advance
	 * \return False if the iterator has completed the loop. It may continue to be used at this point as it will merely restart the loop.
	 */
	bool advance_iterator( vvert_iterator& it ) const;
	
	std::size_t deref_iterator( const vvert_iterator& it ) const;
	std::size_t deref_iterator_left_face( const vvert_iterator& it ) const;
	std::size_t deref_iterator_right_face( const vvert_iterator& it ) const;
	std::size_t deref_iterator_left_edge( const vvert_iterator& it ) const;
	std::size_t deref_iterator_right_edge( const vvert_iterator& it ) const;

	Util::Point get_vertex( const vvert_iterator& it ) const;
	double get_cotan_weight( const vvert_iterator& it ) const;
	double get_mean_value_weight( const vvert_iterator& it ) const;

	/**
	 * Intialize an iterator that visits the faces in the 1-ring of 'vertex'
	 */
	bool init_iterator(vface_iterator& it, std::size_t vertex ) const;
	bool advance_iterator( vface_iterator& it ) const;
	std::size_t deref_iterator( const vface_iterator& it ) const;
	Util::Vector get_normal( const vface_iterator& it ) const;
	// Eigen::Vector4d get_plane( const vface_iterator& it ) const;

	bool init_iterator(fvert_iterator& it, std::size_t face ) const;
	bool advance_iterator( fvert_iterator& it ) const;
	std::size_t deref_iterator( const fvert_iterator& it ) const;

	bool init_iterator(fface_iterator& it, std::size_t face ) const;
	bool advance_iterator( fface_iterator& it ) const;
	std::size_t deref_iterator( const fface_iterator& it ) const;

    /*====================================================
     * Helper Functions
     *====================================================*/
public:
    void getIndicesForFace( size_t tri_index, size_t indicesForFace[3] ) const;
    Util::Point getFaceMidpoint(size_t tri_index);

    /*====================================================
     * Reflection Interface (learn stuff about the mesh)
     *====================================================*/
public:
    /* get a list of all vertices/face indices in the mesh as floats in contiguous
     * memory. Used for rendering so loss of precision unimportant */
    void get_draw_data( float *verts, int *indices ) const;
    void get_draw_normals( float *normals ) const;
    void get_draw_selection( int *selection ) const;
    virtual void drawMesh();
    int  get_edit_count() const;
    void get_face_neighbors(int face_index, size_t neighbors[3]);
    std::vector<size_t> get_vert_neighbours(size_t oldV) const;
    half_edge get_he_for_face(size_t face);
    half_edge get_he_for_vert(size_t vert);
	void flag_editted();

    /* selection interface */
    void select_vert( size_t index );
    void deselect_vert( size_t index );
    void deselect_allVerts();
    bool isSelected( size_t index );

    /* get information about internal state */
    std::size_t get_vert_size() const;
    std::size_t get_face_size() const;

    void test_flip();
	static void test();
	void verify() const;

	void write_to_obj_stream( std::ostream& stream ) const;

	const half_edge& prev( const half_edge& cur ) const;
	const half_edge& next( const half_edge& cur ) const;
	const half_edge& twin( const half_edge& cur ) const;


protected:


	half_edge* find_edge( std::size_t vFrom, std::size_t vTo );
	half_edge* find_twin( std::size_t vFrom, std::size_t vTo );
	std::size_t collapse_edge( std::size_t he );

	void delete_half_edge_impl( std::size_t he );

	template <std::size_t N>
	void delete_half_edges_impl( std::size_t (&edges)[N] );
	
	// Splits a boundary edge into 3 by adding 2 vertices on the specified edge, connecting them to the other vertex. Replaces this face with 3 new ones.
	void split_boundary_edge( std::size_t he, std::size_t (*pOutVertIndices)[2] = NULL, std::size_t (*pOutFaceIndices)[3] = NULL );

    /**
     * a simple helper function to determine if adding a particular face
     * will break mesh manifoldness
     * \param input vertices that could become a face
     * \return if this face will break manifoldness
     */
    bool is_safe_addface( std::size_t v1, std::size_t v2, std::size_t v3 );
    /**
     * Flip the passed in edge to connect the two opposing vertices
     * Flips both the passed in half_edge and its twin
     * \param cur The half edge to flip
     * \return True if successfully flipped, false if edge
     */
    bool flip_edge( half_edge &cur );

    /* =========================================
     * Mesh Collision Tests
     * =========================================*/
public:
    void updateBBox();

    // TODO: update when mesh is changed
    // NOTE: as of right now only used by skeleton animation code
    Util::Point bboxMin;
    Util::Point bboxMax;
    Util::Point bSphereCenter;
    double      bSphereRadius;

    /*========================================
     * Variables for Specific Functions
     *========================================*/
public:
    unsigned int subdiv_iter; // used to track even applications of subdivision

    /*****************************************
     * Mesh Variables
     *****************************************/
public:
	friend class mesh_adjacency;
	// friend class ACCLMesh;
    int edit_count; // increment this value every change

	// TODO: Switch to std::deque to avoid pointer invalidation when adding new half-edges.
	std::vector< half_edge > m_heData;     // All the half-edges that make up the mesh.
	std::vector< std::size_t > m_faceData; // A mapping from face index to an arbitrary half-edge on its boundary.
	std::vector< std::size_t > m_vertData; // A mapping from vertex index to an arbitrary half-edge originating from this vertex. Can be "HOLE_INDEX" for unconnected vertices.
	
    std::vector< bool > m_selected; // if a vertex is selected or not
	std::vector<Util::Point> m_vertices;
	
	struct node{
		double value;
	};

	// friend NavMeshDataBase;
	// TODO: If we need arbitrary data, add a std::map< std::string, vector > that holds the per-vertex data. 
	// TODO: Same for per-half-edge and per-face data.

};

inline Mesh::Mesh()
	: edit_count(0), subdiv_iter(0)
{}

inline half_edge Mesh::get_he_for_face(size_t face)
{
	return this->m_heData[this->m_faceData[face]];
}

inline half_edge Mesh::get_he_for_vert(size_t vert)
{
	return this->m_heData[this->m_vertData[vert]];
}

inline std::size_t Mesh::add_vertex( double x, double y, double z ){
	std::size_t newIndex = m_vertices.size();
	m_vertices.emplace_back( x,y,z );
	m_vertData.push_back( HOLE_INDEX );
	return newIndex;
}

inline std::size_t Mesh::add_vertex( Util::Point v )
{
	return add_vertex(v[0], v[1], v[2]);
}

inline Util::Point Mesh::get_vertex( std::size_t i ) const {
	return m_vertices[i];
}

inline void Mesh::set_vertex( std::size_t i, const Util::Point& v ) {
	m_vertices[i] = v;
}

inline std::size_t Mesh::add_face( std::size_t v1, std::size_t v2, std::size_t v3 ) {
	std::size_t v[] = { v1, v2, v3 };
	return this->add_face( v );
}

inline std::size_t Mesh::collapse_edge( std::size_t v1, std::size_t v2 ){
	return this->collapse_edge( this->find_twin( v1, v2 )->twin );
}

inline bool Mesh::init_iterator( vvert_iterator& it, std::size_t vertex ) const {
	std::size_t vertToHE = m_vertData[ vertex ];
			
	if( vertToHE == HOLE_INDEX )
		return false;

	// Store a pointer to the (arbitrary) first half-edge pointing into the specified vertex. This implies m_heData[m_cur->twin].vert == vertex & m_heData[m_cur->next].vert == vertex.
	// By iterating around the half-edges pointing into 'vertex' we will visit all the vertices in the 1-ring.
	it.m_cur = it.m_end = &m_heData[ m_heData[ vertToHE ].twin ];
	return true;
}

inline bool Mesh::reset_boundary_iterator( vvert_iterator &it ) const {
   const half_edge *cur = it.m_cur;
    do {
        if (it.m_cur->face == HOLE_INDEX)
        {
            it.m_end = it.m_cur;
            return true;
        }
        it.m_cur = &m_heData[ m_heData[ it.m_cur->next ].twin ];
    } while (cur != it.m_cur);

    return false;
}

inline bool Mesh::isBoundaryFace( std::size_t i ) const {
    std::size_t he_index = m_faceData[i];
    const half_edge *he = &m_heData[ he_index ];
    while( he->next != he_index ) {
        if( m_heData[he->twin].face == HOLE_INDEX )
            return true;
        he = &m_heData[ he->next ];
    }
    return false;
}

inline bool Mesh::advance_iterator( vvert_iterator& it ) const {
	it.m_cur = &m_heData[ m_heData[ it.m_cur->next ].twin ];
	return it.m_cur != it.m_end;
}

inline std::size_t Mesh::deref_iterator( const vvert_iterator& it ) const {
	return it.m_cur->vert;
}

inline std::size_t Mesh::deref_iterator_left_face( const vvert_iterator& it ) const {
	return m_heData[it.m_cur->twin].face;
}

inline std::size_t Mesh::deref_iterator_right_face( const vvert_iterator& it ) const {
	return it.m_cur->face;
}

inline std::size_t Mesh::deref_iterator_left_edge( const vvert_iterator& it ) const {
	return it.m_cur->twin;
}

inline std::size_t Mesh::deref_iterator_right_edge( const vvert_iterator& it ) const {
	return m_heData[ it.m_cur->twin ].twin;
}

inline bool Mesh::init_iterator( vface_iterator& it, std::size_t vertex ) const {
	std::size_t vertToHE = m_vertData[ vertex ];
			
	if( vertToHE == HOLE_INDEX )
		return false;

	// Store a pointer to the (arbitrary) first half-edge pointing into the specified vertex. This implies m_heData[m_cur->twin].vert == vertex & m_heData[m_cur->next].vert == vertex.
	// By iterating around the half-edges pointing into 'vertex' we will visit all the vertices in the 1-ring.
	it.m_cur = it.m_end = &m_heData[ m_heData[ vertToHE ].twin ];
	it.m_next = &m_heData[ m_heData[ it.m_cur->next ].twin ];
	return true;
}

inline bool Mesh::advance_iterator( vface_iterator& it ) const {
	it.m_cur = it.m_next;
	it.m_next = &m_heData[ m_heData[ it.m_next->next ].twin ];
	return it.m_cur != it.m_end;
}

inline std::size_t Mesh::deref_iterator( const vface_iterator& it ) const {
	return it.m_cur->face;
}

inline bool Mesh::init_iterator(fvert_iterator& it, std::size_t face ) const {
	assert( face < m_faceData.size() );
	std::size_t faceToHE = m_faceData[ face ];
	assert( faceToHE < m_heData.size() );

	// Store a pointer to the (arbitrary) first half-edge pointing into the specified vertex. This implies m_heData[m_cur->twin].vert == vertex & m_heData[m_cur->next].vert == vertex.
	// By iterating around the half-edges pointing into 'vertex' we will visit all the vertices in the 1-ring.
	it.m_cur = it.m_end = &m_heData[ faceToHE ];
	return true;
}

inline bool Mesh::advance_iterator( fvert_iterator& it ) const {
	it.m_cur = &m_heData[ it.m_cur->next ];
	return it.m_cur != it.m_end;
}

inline std::size_t Mesh::deref_iterator( const fvert_iterator& it ) const {
	return it.m_cur->vert;
}

inline bool Mesh::init_iterator(fface_iterator& it, std::size_t face ) const {
	assert( face < m_faceData.size() );
	std::size_t faceToHE = m_faceData[ face ];
	assert( faceToHE < m_heData.size() );

	// Store a pointer to the (arbitrary) first half-edge pointing into the specified vertex. This implies m_heData[m_cur->twin].vert == vertex & m_heData[m_cur->next].vert == vertex.
	// By iterating around the half-edges pointing into 'vertex' we will visit all the vertices in the 1-ring.
	it.m_cur = it.m_end = &m_heData[ faceToHE ];
	return true;
}

inline bool Mesh::advance_iterator( fface_iterator& it ) const {
	it.m_cur = &m_heData[ it.m_cur->next ];
	return it.m_cur != it.m_end;
}

inline std::size_t Mesh::deref_iterator( const fface_iterator& it ) const {
	return m_heData[ it.m_cur->twin ].face;
}

inline const half_edge& Mesh::prev( const half_edge& cur ) const {
#ifdef USE_PREV
	return m_heData[ cur.prev ];
#else
	return m_heData[ m_heData[ cur.next ].next ];
#endif
}

inline const half_edge& Mesh::next( const half_edge& cur ) const {
	return m_heData[ cur.next ];
}

inline const half_edge& Mesh::twin( const half_edge& cur ) const {
	return m_heData[ cur.twin ];
}

inline void Mesh::select_vert( size_t index ) {
    m_selected[index] = true;
    flag_editted();
}

inline void Mesh::deselect_vert( size_t index ) {
    m_selected[index] = false;
    flag_editted();
}

inline void Mesh::deselect_allVerts() {
    for (int i = 0; i < m_selected.size(); i++) {
        m_selected[i] = false;
    }
    flag_editted();
}

inline bool Mesh::isSelected( size_t index ) {
    return m_selected[index];
}

inline std::size_t Mesh::get_vert_size() const {
    return m_vertData.size();
}

inline std::size_t Mesh::get_face_size() const {
    return m_faceData.size();
}

inline int Mesh::get_edit_count() const{
    return edit_count;
}

inline void Mesh::get_face_neighbors(int face_index, size_t neighbors[3]) {
    fface_iterator fit;
    init_iterator(fit, face_index);
    int i = 0;
    do {
        neighbors[i++] = deref_iterator(fit);
    } while(advance_iterator(fit));

    for (; i < 3; i++)
        neighbors[i] = HOLE_INDEX;
}

inline void Mesh::flag_editted(){
	++edit_count;
}


#endif /* MESH_SRC_MESH_H_ */
