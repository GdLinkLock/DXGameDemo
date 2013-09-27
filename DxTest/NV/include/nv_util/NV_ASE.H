/*********************************************************************NVMH1****
File:
nv_ase.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#ifndef __nv_ase_h__
#define __nv_ase_h__
#include  "STLDefine.h"
  enum LightType { LIGHT_OMNI, 
			   LIGHT_TARGET, 
			   LIGHT_DIRECTIONAL, 
			   LIGHT_FREE,
			   LIGHT_UNKNOWN };
  enum LightShadow { LIGHT_SHADOW_OFF,
			     LIGHT_SHADOW_MAP,
			     LIGHT_SHADOW_RAY,
			     LIGHT_SHADOW_UNKNOWN,};

  enum SpotShape { LIGHT_SPOT_CIRCLE,
			   LIGHT_SPOT_RECT,
			   LIGHT_SPOT_UNKNOWN};

  enum AnimType
  {
		ANIM_LINEAR,
		ANIM_TCB,
		ANIM_BEZIER,
		ANIM_SAMPLE,
  };
namespace ase
{


    // forward declaration
    struct texdata;
    struct mapobj;
    struct matobj;
    struct geomobj;
	struct lightobj;
	struct stripobj;
	struct camobj;

	// add by lance 2003-8-8

	struct lodobj;

    // type definition
	typedef std::vector<mapobj*>    map_array;
    typedef map_array::iterator     map_it;
    
    typedef std::vector<matobj*>    mat_array;
    typedef mat_array::iterator     mat_it;

    typedef std::vector<geomobj*>   geom_array;
    typedef geom_array::iterator    geom_it;

	typedef std::vector<lightobj*>  light_array;
	typedef light_array::iterator	light_it;

	typedef std::vector<camobj*>    cam_array;

	typedef std::map<std::string,texdata*> tex_map;
    typedef tex_map::iterator       tex_it;
    typedef tex_map::const_iterator tex_cit;
    typedef tex_map::value_type     tex_pair;

    // structure definition
    struct texdata
    {
        texdata() : width(0), height(0), pixels(0), components(0), name(0) {}
        ~texdata();

        char *          name;
        int             width;
        int             height;
        int             components;
        unsigned char * pixels;

        // GL stuff - may want to remove that stuff?
        unsigned int          textid;
        unsigned int          pixelfmt;
    };
		struct t_poskey
		{
			int type;
			float pos[3];
			int frmtime;
			//tcb parameters
			float tcbparam[5];
			float bezier[7];
		};
		struct t_rotkey
		{
			int type;
			float quat[4];
			int frmtime;
			//tcb parameters
			float tcbparam[5];
			float bezier[7];
		};

		struct t_sclkey
		{
			int type;
			float scl[7];
			int frmtime;
			//tcb parameters
			float tcbparam[5];
			float bezier[7];

		};

		struct t_fovkey
		{
			int frmtime;
			float fov;
			float far_plane;
			float near_plane;
			float tdist;
		};

    struct animdata
    {
        animdata()  {}
        ~animdata();

        // The anim structure strips out key information. We assume that the 
        // data has been sampled at constant time interval
		// use key info now, by puzzy -2002-1-4

		int firstfrm,lastfrm,tickperfrm,framespeed;


//		int					frmtime;
		std::vector<t_poskey> pos;    // positional data
		std::vector<t_rotkey> rot;    // positional data
		std::vector<t_sclkey> scl;    // positional data

    };

    struct mapobj
    {
        mapobj() : name(0), classname(0), bitmap(0), map_type(0), bitmap_filter(0) {};
        ~mapobj();

        char *          name;
        char *          classname;
        unsigned int    subno;
        float           amount;
        char *          bitmap;
        char *          map_type;
        float           u_offset;
        float           v_offset;
        float           u_tiling;
        float           v_tiling;
		int				map_tile;
        float           angle;
        float           blur;
        float           blur_offset;
        float           noise_amt;
        float           noise_size;
        unsigned int    noise_level;
        float           noise_phase;
        char*           bitmap_filter;
		map_array       map_generic;

		//add by puzzy ,2001-10-23
		float			tmat[9];//to trans, rotate and scale the tex coords
		int				channel;
		int				alpha_src;
		int				mono_channel;//0:rgb,1:alpha
		int				rgb_channel;//0: alpha 1:rgb
		float			anim_speed;
        
	};

    struct matobj
    {
        matobj() : twosided(false), falloff(false),name(0), classname(0), shader(0), xp_type(0) {};
        ~matobj();

        char *          name;
		char *          classname;
		float           ambient[3];
        float           diffuse[4];
        float           specular[4];
		float           shine;
        float           shinestrength;
        float           transparency;
        float           wiresize;
        char *          shader;
        float           xp_falloff;
        float           selfillum;
        bool            twosided;
        bool            falloff;
        char *          xp_type;
        
        mat_array       submat;

        map_array       map_ambient;
        map_array       map_generic;
        map_array       map_diffuse;
        map_array       map_specular;
        map_array       map_bump;
        map_array       map_selfillum;
        map_array       map_reflect;
        map_array       map_shine;
        map_array       map_shinestrength;
        map_array       map_opacity;
        map_array       map_refract;
        map_array       map_filtercolor;
    };

     
     //  in the the vec array
    struct patch
    {
        int numedges;       // 4 for quad, 3 for tri
        int edge[4];        // 4 indices to reference edges
        int interior[4];    // 4 indices to reference inner control vectors
        int smg;            // smoothing group idx
        int mtlid;
    };

	struct animgeom
	{
		geomobj*	geom;
		int			frmtime;
	};

	//by sssa2000 090724
	typedef enum {
		BOUND_SPHERE,
		BOUND_CAPSULE,
		BOUND_BOX
	} BOUND_TYPE;

    struct geomobj
    {
        geomobj();
        ~geomobj();

        typedef enum {
            polygonal,
            patched
        } geomtype;

        geomtype        type;   // tells if the geomobj is polygonal or patched
		int				timevalue;//for animation mesh

        char *          name;   // object name
		char *			classname;
		char *			parentname;
        geomobj *       parent; // ref to the parent geomobj
        geom_array      children;
		geom_array		anim_mesh;//animation meshes

        float           bone_offset_tm[16]; // bone offset transform
        float           tm[16]; // world transform
        float           rtm[16]; // 4x4 transform from the parent

		float			pos[3];
		float			scale[3];
		float			rotaxis[3];
		float			rotangle;

        // polygonal
        float *         v;      // vertices
		float *         n;      // normals
		float *         Tn;      // tangent  add by LH 20100720
        float *         t;      // texcoords

		float *			t1;		// texcoord 1  by sssa2000 090925
		float *			t2;		// texcoord 2  by sssa2000 090925
		float *			t3;		// texcoord 3  by sssa2000 090925

        float *         c;      // vertex colors
		unsigned int    numn;   // number of vertex normals
		unsigned int    numTn;   // number of vertex tangents add by LH 20100720
        unsigned int    numt;   // number of texcoords
		unsigned int    numt1;   // number of texcoord1 by sssa2000 090926
		unsigned int    numt2;   // number of texcoord2 by sssa2000 090926
		unsigned int    numt3;   // number of texcoord3 by sssa2000 090926

        unsigned int    numv;   // number of vertices
        unsigned int    numc;   // number of vertices color
		unsigned int *  fvn;    // indices for the vertex normals indexed by face index
		unsigned int *  fvTn;    // indices for the vertex normals indexed by face index
        
        unsigned int *  fsubmat; // submaterial per face
        unsigned int *  f;      // faces
        unsigned int *  tf;     // texture faces
        unsigned int *  tf1;    // texture faces
		unsigned int *  cf;		// color faces, add by puzzy ,2001-11-6
        float *         fn;     // face normals
        unsigned int *  fmapv;  // face map vertices
        unsigned int    numf;   // number of faces
        unsigned int    numtf;  // number of texture faces
        unsigned int    numtf1;  // number of texture faces
		unsigned int	numcf;	// number of color faces, add by puzzy 2001-11-6
         int *  smg;    // smoothing group

        // patches
        int             numverts;
        float *         verts;
        int             numvecs;
        float *         vecs;
        int             numedges;
        int *           edges;
        int             numpatches;
        patch *         patches;     
                                     

        int             numtvchannels; // normally 2, where channel 0 is vertex colors 
                                     // and channel 1 is texcoords
        int *           numtvverts;  // number of tvverts per channel
        float **        tvverts;     // array of tvverts per channel
        int **          tvpatches;   // array of indices to tvverts per channel 
                                     // (defining a tvpatch (dim = 4))
        
        // animation
        animdata *      anim;   // ref to the animation data

        // material
        int             matidx; // index in the material array

        // vertex weighting data - may have to be reordered
        unsigned int    numbv;      // number of blended vertices
        float *         bv;         // blended vertices {x,y,z,w} where w is the weight
        float **        bmatref;    // per blended vertex geom obj matrix tm reference
        unsigned int *  vbv;        // vertex indices
        geomobj **      bgeomref;   // per blended vertex geom obj reference

		//strip optimal by hardcore3d engine's 3dsmax exporter
		stripobj*		strip;

		//lod information by lance(moved to t_strip_mat)
		//H3DVector<lodobj*> lodlist;


		//包围体数据
		BOUND_TYPE bound_type;
		float bound_data[6]; //对于包围球来说,前3个是球心,第四个是半径.对于胶囊来说,前三个是位置,第四个是高度,第五个是半径

		int CreateTc(int idx,unsigned int number_of_elem);

    };

	//add by hardcore3d engine's ase exporter
	struct lightobj
	{


		int type;
		char* name;
		//omni,include all kinds of lights
		float	r,g,b;
		float	intensity;
		float	attnstart,attnend;
		float	pos[3];
		float	target[3];
		float	mapbias;

		int		shadow;
		int		uselight;
		int		useglobal;
		int		overshoot;


		int		numexclude;
		int		include ;
		int		affect_illum;
		int		affect_shadow;
		std::vector<char*> exclude_list;


		float	spot_hotspot;
		float	spot_falloff;
		int		spot_shape;
		char	spot_projmap[256];
		float	spot_aspect;
		int		hasProjmap;

		
		
		//spot
		int spottype;

		//direction
		float tm[16];

		//animation
		animdata* anim;

		lightobj()
		{
			attnstart = 200;attnend = 800;r=g=b=1.0f;intensity = 1.0f;
			pos[0] = 0;pos[1] = 0;pos[2] = 0;name=0;
			hasProjmap=0;
			spot_projmap[0]=0;
			spot_hotspot = 45;
			spot_falloff = 45;
			spot_shape = 0;
			target[0] = target[1] = target[2] = 0;
			shadow = uselight=useglobal=overshoot=	numexclude=	include =affect_illum=affect_shadow=0;
			anim = 0;

		};
	~lightobj()
	{
		if (name) delete []name;

		if (exclude_list.size())
		{
			for (size_t i=0;i<exclude_list.size();++i)
			{
				if (exclude_list[i])
					delete [](exclude_list[i]);
			}
		}

		if (anim)
			delete anim;
	};



	};

	struct camobj
	{
		camobj()
		{
			name=0;
			anim = 0;
		};

		~camobj()
		{
			if (name)
				delete []name;

			if (anim)
				delete anim;
		}

		float pos[3];
		float axis[3]; //should to change to EULAR
		float axisangle;
		float fov;
		float far_plane;
		float near_plane;
		float tdist;
		char* name;
		animdata* anim;
		std::vector<t_fovkey> cameraAni;
	};


	struct t_strip
	{
		int smooth;
		int submat;
		std::vector<int> elems;
	};

	struct t_weight
	{
		t_weight(){numbone = 0;};
		~t_weight()
		{
			if (numbone)
			{
				for (int i=0;i<numbone;++i)
					delete []bonenames[i];

				delete []bonenames;
				delete []weights;
			}

		};

		int numbone;
		char** bonenames;
		float * weights;
	};

	typedef std::vector<t_strip*> strip_array;
	struct t_strip_mat
	{
		t_strip_mat():v(0),n(0),t(0),t2(0),c(0),Tn(0){numVert=0;wv = NULL;};
		int submat;
		int numVert;
		//加一个UV
		float *v,*t,*n,*c,*Tn,*t2;
		strip_array strips;

		// add by lance 2004-2-24, for lod
		std::vector<strip_array*> lods;

		// add by lance 2004-3-2,  for another lod...
		std::vector<lodobj*> lodlist;
		
		~t_strip_mat();

		t_weight*	wv;

	};
	typedef std::vector<t_strip_mat*> stripmat_array;
	struct stripobj
	{
		stripobj(){hasTvert = hasTvert2 = hasNormal = hasColor = hasTangent= hasPMSign= 0;	};
		~stripobj()
		{

			
			t_strip_mat* s;
			t_strip* str;
			for (size_t i=0;i<strip_mat.size();++i)
			{
				s = strip_mat[i];
				for (size_t j=0;j<s->strips.size();++j)
				{
					str = s->strips[j];
					delete str;
				}
				if (s->numVert)
				{
					if (hasTvert) delete []s->t;
					if(hasTvert2) delete [] s->t2;
					if (hasNormal) delete []s->n;
					if (hasTangent) delete []s->Tn;
					if (hasColor) delete []s->c;
					delete []s->v;
					
				}
				if (s->wv)
					delete []s->wv;

				delete s;
					
			}
		}
		//添加手相性读取lh
		int	hasTvert,hasTvert2,hasNormal,hasColor,hasTangent, hasPMSign;
		int matidx;

		//std::vector<t_strip*> strips;
		stripmat_array strip_mat;
	};

	//------------------------------------------------




    struct model
    {
        model() : time(0),name(0),firstfrm(0),lastfrm(0),tickperfrm(0){}
        ~model();

        char *      name;
        geom_array  root_geom;
        geom_array  geom;
		light_array light;
		cam_array	cam;
        mat_array   mat;
        tex_map     tex;
        int         time;
		int			firstfrm;
		int			lastfrm;
		int			framespeed;
		int			tickperfrm;
    };

    extern model * load(const char * filename, float scale);
	extern model* load(const char* name,float scale,int bWorld);
    extern model * load(const char * buf, unsigned int size, float scale,int bWorld);
	
    extern geomobj * get_geomobj(model * m, const char * name);
    extern bool load_tex(const char * datapath, model * m);

	extern int    save_h3dfile(const char* name,model* m);
//	extern model*  read_h3dfile(const char* name);






	//**********------------------h3d dist file format-------------------*********
	struct t_h3d_obj_header
	{
		t_h3d_obj_header()
		{
			
			memset(this,0,sizeof(t_h3d_obj_header));
			parent = -1;
		};

		unsigned int	obj_off;
		unsigned int	obj_size;

//		unsigned int	name_off;
		int				name_length;//obj 名字的长度,包括NULL TERMINATED

        int		        type;   // tells if the geomobj is polygonal or patched
		int				timevalue;//for animation mesh

        int				parent; // ref to the parent geomobj
        //int				children;//geom_array      children;
//		unsigned int	children_off;//---->children[]--int[]
		int				num_children;
		//int				anim_mesh;//geom_array		anim_mesh;//animation meshes
//		unsigned int	anim_obj_off;//----->anim_obj[]-->所有ANIMOBJ的序号  int[]
		int				num_anim_obj;

        float           bone_offset_tm[16]; // bone offset transform
        float           tm[16]; // world transform
        float           rtm[16]; // 4x4 transform from the parent

		float			pos[3];
		float			scale[3];
		float			rotaxis[3];
		float			rotangle;

        // polygonal
//		unsigned int	v_off;
//		unsigned int	t_off;
//		unsigned int	c_off;
//		unsigned int	n_off;
        unsigned int    numn;   // number of vertex normals
        unsigned int    numt;   // number of texcoords
        unsigned int    numv;   // number of vertices
        unsigned int    numc;   // number of vertices color

    //  unsigned int    fvn_off;    // indices for the vertex normals indexed by face index
        
    //  unsigned int    fsubmat_off; // submaterial per face
    //  unsigned int    f_off;      // faces
    //  unsigned int    tf_off;     // texture faces
	//	unsigned int    cf_off;		// color faces, add by puzzy ,2001-11-6
    //  unsigned int    fn_off;     // face normals
	//  unsigned int *  fmapv;  // face map vertices
        unsigned int    numf;   // number of faces
        unsigned int    numtf;  // number of texture faces
		unsigned int	numcf;	// number of color faces, add by puzzy 2001-11-6
        //unsigned int    smg_off;    // smoothing group

 

        int             numtvchannels; // normally 2, where channel 0 is vertex colors 
                                     // and channel 1 is texcoords
        //int    numtvverts;  // number of tvverts per channel---int[]
		unsigned int	numtvverts_off;
       // float **        tvverts;     // array of tvverts per channel----float[channel][number]

		//int **          tvpatches;   // array of indices to tvverts per channel 
                                     // (defining a tvpatch (dim = 4))
        
        // animation
		unsigned int	has_anim_track;
        //animdata *      anim;   // ref to the animation data

        // material
        int             matidx; // index in the material array

		//strip optimal by hardcore3d engine's 3dsmax exporter
		unsigned int    has_strip;
		//stripobj*		strip;
	};
	//-----HEADER后面紧跟着所有OBJECT数据

	struct t_h3d_animdata
	{

		int					firstfrm;
		int					lastfrm;
		int					tickperfrm;
		int					framespeed;
		int					frmtime;

		int					numpos;
		int					numrot;
		int					numscl;
	};



	//strip in dist format
	struct t_h3d_strip
	{
		int				hasTvert;
		int				hasNormal;
		int				hasColor;

		int				matidx;
		unsigned int	strip_num;

	};
	struct t_h3d_stripmat
	{
		int				submat;
		int				numvert;
		int				num_elemstrip;
	};

	struct t_h3d_elemstrip
	{
		int				smooth;
		int				submat;
		int				numelem;
//		unsigned int	elems_off;
	};

	struct t_h3d_lightheader
	{
		int					light_off;
		int					light_size;
		t_h3d_lightheader()
		{
			memset(this,0,sizeof(t_h3d_lightheader));
		};

		int					type;
		unsigned int		name_length;

		//omni,include all kinds of lights
		float	r,g,b;
		float	intensity;
		float	attnstart,attnend;
		float	pos[3];
		float	target[3];
		float	mapbias;

		int		shadow;
		int		uselight;
		int		useglobal;
		int		overshoot;


		int		numexclude;
		int		include ;
		int		affect_illum;
		int		affect_shadow;
		int		num_exclude_list;


		float	spot_hotspot;
		float	spot_falloff;
		int		spot_shape;
		unsigned int spot_projmap_length;
		float	spot_aspect;
		int		hasProjmap;

		
		
		//spot
		int spottype;

		//direction
		float tm[16];
	};

/*	map info
	{
		unsigned int name_length
		unsigned int classname_length
		unsigned int subno
		unsigned int bitmap_length
		unsigned int map_type_length
        float           u_offset;
        float           v_offset;
        float           u_tiling;
        float           v_tiling;
		int				map_tile;
        float           angle;
        float           blur;
        float           blur_offset;
        float           noise_amt;
        float           noise_size;
        unsigned int    noise_level;
        float           noise_phase;
		unsigned int	bitmap_filter_length;
		float			tmat[9];//to trans, rotate and scale the tex coords
		int				channel;
		int				alpha_src;
		int				mono_channel;//0:rgb,1:alpha
		int				rgb_channel;//0: alpha 1:rgb
		float			anim_speed;

		map_generic1
		{
		}
		map_generic2
		{
		}
		...
		...
		...
		
		
	}
	*/
	/*文件结构：
	{
		HEADER
		{
			NUM_OBJ,
			NUM_LIGHT,
			NUM_CAM,
			NUM_...
		}



		MATERIAL1
		{
			unsigned int name_length
			unsigned int classname_length
			float           ambient[3];
			float           diffuse[4];
			float           specular[4];
			float           shine;
			float           shinestrength;
			float           transparency;
			float           wiresize;
			float           xp_falloff;
			float           selfillum;
			int             twosided;
			int	            falloff;

			map_ambient1
			{
			}
			map_ambient2
			{
			}
			..
			..
			map_generic1
			{
			}
			map_generic1
			{
			}


			submat1
			{
			}
			submat2
			{
			}
			...
			...

		}

		GEOMOBJ1
		{
			t_h3d_obj_header header

			NAME_LENGTH
			{
				CHAR NAME[]
			}
			...
			...


			{
				UNSIGNED INT CHILDREN_OBJ_IDX[]
			}

			


			VERTEX_BUFFER
			{
				FLOAT [NUM_V*3]
			}



			TEX_VERTEX_BUFFER
			{
				FLOAT [NUM_T*3]
			}



			NORMAL_BUFFER
			{
				FLOAT [NUM_N*3]
			}



			COLOR_BUFFER
			{
				FLOAT [NUM_C*3]
			}


			{
				//FACE INFO
				FACE_VERTEX_IDX
				{
					UNSIGNED INT[NUM_F*3]
				}
				FACE_NORMALS
				{
					FLOAT [NUM_F*3]
				}
				FACE_VERTEX_NORMAL_IDX
				{
					UNSIGNED INT [NUM_F*3]
				}
				FACE_SUBMAT
				{
					UNSIGNED INT [NUM_F]
				}
				FACE_SMOOTH_GROUP
				{
					INT[NUM_F]
				}
			}


			{
				//TEXTURE FACE VERTEX INFO
				UNSIGNED INT [NUM_TF*3]

			}

			{
				//COLOR FACE INFO
				UNSIGNED INT [NUM_CF*3]
			}
			

			{
				INT NUM_TV_VERTS[NUM_TV_CHANNELS]

				TEXTURE VERTEX CHANNEL VERTICES
				{
					FLOAT VT_CHANNEL0[ NUM_TV_VERTS[0]*3 ]
					FLOAT VT_CHANNEL1[ NUM_TV_VERTS[1]*3 ]
					FLOAT VT_CHANNEL2[ NUM_TV_VERTS[2]*3 ]
					...
					...
				}
			}


			ANIM_DATA
			{
				firstfrm
				lastfrm
				tickperfrm
				framespeed

				NUM_POS
				POS_KEYS
				{
					t_poskey[NUM_POS]
				}
				NUM_ROT
				ROT_KEYS
				{
					t_rotkey[NUM_ROT]
				}
				NUM_SCL
				SCL_KEYS
				{
					t_sclkey[NUM_SCL]
				}
			}


			STRIP
			{
				matidx
				hasTvert
				hasNormal
				hasTangent
				hasColor

				strip_num

				STRIPMATINFO
				{
					submat
					NUM_VERT
					VERTEX_BUFFER
					{
						FLOAT [NUM_VERT*3]
					}
					TEX_VERT_BUFFER
					{
					}
					NORMAL_BUFFER
					{
					}
					COLOR_BUFFER
					{
					}

					num_elemstrip
					ELEMSTRIP_INFO
					{
						smooth
						submat
						numelem
						{
							UNSIGNED INT ELEMS[NUMELEM]
						}
					}
				}STRIPMATINFO
			}STRIP


			{
				ANIM_GEOMOBJ1
				{
					...
					...
					NAME_LENGTH
					{
						CHAR NAME[]
					}
					...
					...
					NUM_CHILDREN
					{
						UNSIGNED INT CHILDREN_OBJ_IDX[]
					}
				}
				ANIM_GEOMOBJ2
				{
				}
			}

		}//geomobj1

		..geomobj2...
		..geomobj3...
	}
	*/
	struct t_h3d_header//ase场景二进制文件
	{
		t_h3d_header()
		{
			memset (this,0,sizeof(t_h3d_header));
		};

		int				version;
		int				type;//0: bin format of ASE, 1:bin format of ch3modelbaseT
		char			name[64];//文件内部名称
		int				file_size;

		//animation info
		int				firstfrm;
		int				lastfrm;
		int				framespeed;
		int				tickperfrm;

		//geometry ,light,camera and material info
		int				obj_num ;
		int				obj_off ;
		int				obj_size;

		int				light_num;
		int				light_off;
		int				light_size;

		int				cam_num;
		int				cam_off;
		int				cam_size;

		int				mat_num;
		int				mat_off;
		int				mat_size;
	};



	struct t_h3d_matobj
	{
		t_h3d_matobj()
		{
			memset(this,0,sizeof(t_h3d_matobj));
		};
		int				mat_off;
		int				mat_size;
		unsigned int name_length;
		unsigned int classname_length;
		float           ambient[3];
		float           diffuse[4];
		float           specular[4];
		float           shine;
		float           shinestrength;
		float           transparency;
		float           wiresize;
		float           xp_falloff;
		float           selfillum;
		int             twosided;
		int	            falloff;

		
		unsigned int	num_ambient;
		unsigned int	num_generic;
		unsigned int	num_diffuse;
		unsigned int	num_specular;
		unsigned int	num_bump;
		unsigned int	num_selfillum;
		unsigned int	num_reflect;
		unsigned int	num_shine;
		unsigned int	num_opacity;
		unsigned int	num_refract;
		unsigned int	num_filtercolor;

		unsigned int	num_submat;
	};

	struct t_h3d_mapobj
	{
		t_h3d_mapobj()
		{
			memset(this,0,sizeof(t_h3d_mapobj));
		};

		int				map_off;
		int				map_size;
		unsigned int name_length;
		unsigned int classname_length;
		unsigned int subno;
		float			amount;
		unsigned int bitmap_length;
		unsigned int map_type_length;
        float           u_offset;
        float           v_offset;
        float           u_tiling;
        float           v_tiling;
		int				map_tile;
        float           angle;
        float           blur;
        float           blur_offset;
        float           noise_amt;
        float           noise_size;
        unsigned int    noise_level;
        float           noise_phase;
		unsigned int	bitmap_filter_length;
		float			tmat[9];//to trans, rotate and scale the tex coords
		int				channel;
		int				alpha_src;
		int				mono_channel;//0:rgb,1:alpha
		int				rgb_channel;//0: alpha 1:rgb
		float			anim_speed;
		int				num_genericmap;

		//not use currently
		int				polyoffset;
		int				rgbGen;
		float			rgbGenParam[5];
		int				tcGen;
		int				alphaGen;
		int				num_tcModifiler;
	};


	struct t_h3d_tcModifiler
	{
		//not be used currently
		int				tcMode;
		float			param[10];
	};


	struct lodobj
	{
		lodobj();
		~lodobj();

		float depth;					// from this depth current lod should be used
		unsigned short num_faces;		// how many faces this lod contain
		unsigned short striplength;		// length of the triangle strip (only 1 strip)
		unsigned short indicescount;	// index count of the triangles
		unsigned short* pStrip;			// pointer to the strip buffer
		unsigned short* pIndices;		// pointer to the triangles list buffer
	};

}


#endif /* __nv_ase_h__ */