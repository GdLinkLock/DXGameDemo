/*********************************************************************NVMH1****
File:
nv_ase.cpp

Comments:


******************************************************************************/

#include "sstream"
#include "string"
#include <iostream>
#include <sys/stat.h>



#ifndef __nv_ase_h__
//#   include <nv_ase.h>
#include "./include/nv_util/nv_ase.h"
#endif // __nv_ase_h__

#ifndef GLH_LINEAR_H
//#   include "glh\glh_linear.h"
#include "./Include/glh/glh_linear.h"
#endif


// File mapper name
#define NV_ASE_MAPPER_NAME "NvASEFileMapper"

#define TOKEN_SIZE 4096

//#include "Globals.h"
//#include "IO_LastIncludeFile.h"


#pragma  warning(push)
#pragma  warning(disable : 4018)
#pragma  warning(disable : 4244)
#pragma  warning(disable : 4267)

void mat33_mmult(float* a,float* b,float* c)
{
	c[0] = a[0]*b[0] + a[3]*b[1] + a[6]*b[2];
	c[1] = a[1]*b[0] + a[4]*b[1] + a[7]*b[2];
	c[2] = a[2]*b[0] + a[5]*b[1] + a[8]*b[2];

	c[3] = a[0]*b[3] + a[3]*b[4] + a[6]*b[5];
	c[4] = a[1]*b[3] + a[4]*b[4] + a[7]*b[5];
	c[5] = a[2]*b[3] + a[5]*b[4] + a[8]*b[5];

	c[6] = a[0]*b[6] + a[3]*b[7] + a[6]*b[8];
	c[7] = a[1]*b[6] + a[4]*b[7] + a[7]*b[8];
	c[8] = a[2]*b[6] + a[5]*b[7] + a[8]*b[8];

}
namespace ase
{
	int     g_stream_cnt;
	int     g_stream_max_cnt;
	char    g_token[TOKEN_SIZE];
	float   g_scale_hack = 0.0002f;
	char    g_keyword[TOKEN_SIZE];
	char    g_leftover[TOKEN_SIZE];





	// ase tokens
	const char tok_close[]              = "}";
	const char tok_begin[]              = "{";
	const char tok_group[]              = "GROUP";
	const char tok_geomobject[]         = "GEOMOBJECT";
	const char tok_helperobject[]       = "HELPEROBJECT";
	const char tok_helpclass[]			= "HELPER_CLASS";
	const char tok_scene[]              = "SCENE";
	const char tok_shapeobject[]        = "SHAPEOBJECT";
	const char tok_cameraobject[]       = "CAMERAOBJECT";
	const char tok_lightobject[]        = "LIGHTOBJECT";

	const char tok_node_name[]          = "NODE_NAME";
	const char tok_node_parent[]        = "NODE_PARENT";
	const char tok_node_tm[]            = "NODE_TM";
	const char tok_bound_sphere_info[]	= "BOUND_SPHERE_PARAM"; //by sssa2000 090724 为了能够读出碰撞体的信息
	const char tok_bound_capsule_info[]	= "BOUND_CAPSULE_PARAM"; //by sssa2000 090724 为了能够读出碰撞体的信息
	const char tok_bound_box_info[]		= "BOUND_BOX_PARAM"; //by sssa2000 090724 为了能够读出碰撞体的信息

	const char tok_tm_animation[]       = "TM_ANIMATION";
	const char tok_tm_row0[]            = "TM_ROW0";
	const char tok_tm_row1[]            = "TM_ROW1";
	const char tok_tm_row2[]            = "TM_ROW2";
	const char tok_tm_row3[]            = "TM_ROW3";


	const char tok_mesh[]               = "MESH";
	const char tok_mesh_numvertices[]   = "MESH_NUMVERTEX";
	const char tok_mesh_numfaces[]      = "MESH_NUMFACES";
	const char tok_mesh_numtvertices[]  = "MESH_NUMTVERTEX";
	const char tok_mesh_numtvertlist[]  = "MESH_TVERTLIST";
	const char tok_mesh_numcvertex[]    = "MESH_NUMCVERTEX";
	const char tok_mesh_numtvfaces[]    = "MESH_NUMTVFACES";
	const char tok_mesh_vertexlist[]    = "MESH_VERTEX_LIST";
	const char tok_mesh_facelist[]      = "MESH_FACE_LIST";
	const char tok_mesh_normals[]       = "MESH_NORMALS";
	const char tok_mesh_tangents[]       = "MESH_TANGENTS";
	const char tok_mesh_tfacelist[]     = "MESH_TFACELIST";
	const char tok_mesh_tvert[]         = "MESH_TVERT";
	const char tok_mesh_vertex[]        = "MESH_VERTEX";
	const char tok_mesh_face[]          = "MESH_FACE";
	const char tok_mesh_facenormal[]    = "MESH_FACENORMAL";
	const char tok_mesh_vertexnormal[]  = "MESH_VERTEXNORMAL";
	const char tok_mesh_vertextangent[]  = "MESH_VERTEXTANGENT";
	const char tok_mesh_tface[]         = "MESH_TFACE";    
	const char tok_mesh_facemaplist[]   = "MESH_FACEMAPLIST";
	const char tok_mesh_facemap[]       = "MESH_FACEMAP";
	const char tok_mesh_facemapvert[]   = "MESH_FACEMAPVERT";
	const char tok_mesh_animation[]     = "MESH_ANIMATION";
	const char tok_mesh_vertcol[]       = "MESH_VERTCOL";
	const char tok_mesh_mappingchannel[] ="MESH_MAPPINGCHANNEL";
	const char tok_mesh_numcvfaces[]    = "MESH_NUMCVFACES";
	const char tok_mesh_cvertlist[]     = "MESH_CVERTLIST";
	const char tok_mesh_cfacelist[]     = "MESH_CFACELIST";
	const char tok_mesh_cface[]         = "MESH_CFACE";

	//--- light tokens , add by puzzy ---
	const char tok_light[]				= "LIGHTOBJECT";
	const char tok_light_type[]			= "LIGHT_TYPE";
	const char tok_light_settings[]		= "LIGHT_SETTINGS";
	const char tok_light_color[]		= "LIGHT_COLOR";
	const char tok_light_intens[]		= "LIGHT_INTENS";
	const char tok_light_attnstart[]	= "LIGHT_ATTNSTART";
	const char tok_light_attnend[]		= "LIGHT_ATTNEND";
	const char tok_light_shadow[]		= "LIGHT_SHADOWS";
	const char tok_light_mapbias[]		= "LIGHT_MAPBIAS";
	const char tok_light_hotspot[]		= "LIGHT_HOTSPOT";
	const char tok_light_falloff[]		= "LIGHT_FALLOFF";
	const char tok_light_aspect[]		= "LIGHT_ASPECT";
	const char tok_light_projmap[]		= "LIGHT_PROJMAP";
	const char tok_light_spotshape[]		= "LIGHT_SPOTSHAPE";
	//---- strip tokens , add by hardcore3d 2001-12-5
	const char tok_strip[]				= "STRIPS";
	const char tok_strip_tvert[]		= "STRIP_TVERT";
	const char tok_strip_tvert2[]		= "STRIP_TVERT2";
	const char tok_strip_normal[]		= "STRIP_NORMAL";
	const char tok_strip_tangent[]		= "STRIP_TANGENT";
	const char tok_strip_tangent_sign[] = "STRIP_PLUSMINUS_SIGN";
	const char tok_strip_vertcolor[]	= "STRIP_VERTCOLOR";
	const char tok_strip_numverts[]		= "STRIP_NUMVERTS";
	const char tok_strip_verts[]		= "STRIP_VERTS";
	const char tok_strip_vert3ds[]		= "Vert3ds";
	const char tok_strip_vertweight[]	= "V_weight";
	const char tok_strip_t[]			= "UV:";
	const char tok_strip_n[]			= "Normal:";
	const char tok_strip_vc[]			= "VertColor:";
	const char tok_strip_numsubmtls[]	= "STRIP_NUMSUBMTLS";
	const char tok_strip_submtls[]		= "STRIP_SUBMTLS";
	const char tok_strip_submat[]		= "SUBMAT";
	const char tok_strip_submat_strips[]= "SUBMAT_STRIPS";
	const char tok_strip_smooth[]		= "SMOOTHGROUP";
	const char tok_strip_numstrips[]	= "NUM_STRIPS";
	const char tok_strip_token[]		= "Strip";
	const char tok_strip_submatverts[]	= "SUBMAT_VERTS";
	const char tok_strip_influencbone[]	= "Influence_Bone:";


	const char tok_strip_submat_strips_lod[]  = "SUBMAT_STRIPS_LOD";
	//--- lod tokens , add by lance 2003-8-12
	/// add AGAIN at 2003-12-17
	const char tok_lod[]				= "LEVELOFDETAIL";
	const char tok_lod_3dsmax[]         = "LEVELOFDETAIL_MAX";
	const char tok_lod_numlevels[]		= "NUM_LEVELS";
	const char tok_lod_depth[]			= "DEPTH";
	const char tok_lod_numfaces[]		= "NUM_FACES";
	const char tok_lod1[]				= "LOD";
	const char tok_lod_striplength[]	= "STRIP_LENGTH";
	const char tok_lod_strip[]			= "STRIP";
	const char tok_lod_trianglecount[]	= "TRIANGLE_COUNT";
	const char tok_lod_triangles[]		= "TRIANGLES";
	const char tok_lod_triangle[]		= "TRIANGLE";

	// custom tokens to exportXml patch data
	const char tok_patchmesh[]              = "PATCHMESH";
	const char tok_patchmesh_numverts[]     = "PATCHMESH_NUMVERTS";
	const char tok_patchmesh_numvecs[]      = "PATCHMESH_NUMVECS";
	const char tok_patchmesh_numpatches[]   = "PATCHMESH_NUMPATCHES";
	const char tok_patchmesh_numedges[]     = "PATCHMESH_NUMEDGES";
	const char tok_patchmesh_vertex_list[]  = "PATCHMESH_VERTEXLIST";
	const char tok_patchmesh_vertex[]       = "PATCHMESH_VERTEX";
	const char tok_patchmesh_edge_list[]    = "PATCHMESH_EDGE_LIST";
	const char tok_patchmesh_edge[]         = "PATCHMESH_EDGE";
	const char tok_patchmesh_vector_list[]  = "PATCHMESH_VECTOR_LIST";
	const char tok_patchmesh_vector[]       = "PATCHMESH_VECTOR";

	const char tok_patchmesh_patch_list[]   = "PATCHMESH_PATCH_LIST";
	const char tok_patchmesh_patch[]        = "PATCHMESH_PATCH";

	const char tok_patchmesh_tvchannel_list[] = "PATCHMESH_TVCHANNEL_LIST";
	const char tok_patchmesh_numtvchannels[] = "PATCHMESH_NUMTVCHANNELS";
	const char tok_patchmesh_tvchannel[]    = "PATCHMESH_TVCHANNEL";

	const char tok_patchmesh_tvvert_list[]  = "PATCHMESH_TVVERTLIST";
	const char tok_patchmesh_numtvverts[]   = "PATCHMESH_NUMTVVERTS";
	const char tok_patchmesh_tvvert[]       = "PATCHMESH_TVVERT";
	const char tok_patchmesh_tvpatch_list[] = "PATCHMESH_TVPATCH_TVPATCH_LIST";
	const char tok_patchmesh_tvpatch[]      = "PATCHMESH_TVPATCH";


	// Custom tokens to exportXml Physique data
	const char tok_mesh_vertex_weightlist[] = "VERTEX_WEIGHTLIST";
	const char tok_mesh_vertex_weight[] = "VERTEX_WEIGHT";

	const char tok_control_pos_track[]  = "CONTROL_POS_TRACK";    
	const char tok_control_rot_track[]  = "CONTROL_ROT_TRACK";    
	const char tok_control_scale_track[] = "CONTROL_SCALE_TRACK";    

	const char tok_control_pos_sample[] = "CONTROL_POS_SAMPLE";
	const char tok_control_rot_sample[] = "CONTROL_ROT_SAMPLE";
	const char tok_control_scale_sample[] = "CONTROL_SCALE_SAMPLE";

	const char tok_material_ref[]       = "MATERIAL_REF";
	const char tok_material_list[]      = "MATERIAL_LIST";
	const char tok_material_count[]     = "MATERIAL_COUNT";
	const char tok_material[]           = "MATERIAL";
	const char tok_material_name[]      = "MATERIAL_NAME";
	const char tok_material_class[]     = "MATERIAL_CLASS";
	const char tok_material_ambient[]   = "MATERIAL_AMBIENT";
	const char tok_material_diffuse[]   = "MATERIAL_DIFFUSE";
	const char tok_material_specular[]  = "MATERIAL_SPECULAR";
	const char tok_material_shine[]     = "MATERIAL_SHINE";
	const char tok_material_shinestrength[] = "MATERIAL_SHINESTRENGTH";
	const char tok_material_transparency[] = "MATERIAL_TRANSPARENCY";
	const char tok_material_wiresize[]  = "MATERIAL_WIRESIZE";
	const char tok_material_shading[]   = "MATERIAL_SHADING";
	const char tok_material_xp_falloff[] = "MATERIAL_XP_FALLOFF";
	const char tok_material_selfillum[] = "MATERIAL_SELFILLUM";
	const char tok_material_twosided[]  = "MATERIAL_TWOSIDED";
	const char tok_material_falloff[]   = "MATERIAL_FALLOFF";
	const char tok_material_xp_type[]   = "MATERIAL_XP_TYPE";
	const char tok_submaterial[]        = "SUBMATERIAL";

	const char tok_map_generic[]        = "MAP_GENERIC";
	const char tok_map_ambient[]        = "MAP_AMBIENT";
	const char tok_map_diffuse[]        = "MAP_DIFFUSE";
	const char tok_map_specular[]       = "MAP_SPECULAR";
	const char tok_map_shine[]          = "MAP_SHINE";
	const char tok_map_shinestrength[]  = "MAP_SHINESTRENGTH";
	const char tok_map_selfillum[]      = "MAP_SELFILLUM";
	const char tok_map_opacity[]        = "MAP_OPACITY";
	const char tok_map_filtercolor[]    = "MAP_FILTERCOLOR";
	const char tok_map_bump[]           = "MAP_BUMP";
	const char tok_map_reflect[]        = "MAP_REFLECT";
	const char tok_map_refract[]        = "MAP_REFRACT";

	const char tok_map_name[]           = "MAP_NAME";
	const char tok_map_class[]          = "MAP_CLASS";
	const char tok_map_subno[]          = "MAP_SUBNO";
	const char tok_map_amount[]         = "MAP_AMOUNT";
	const char tok_map_type[]           = "MAP_TYPE";

	const char tok_map_bitmap[]         = "BITMAP";

	const char tok_map_u_offset[]       = "UVW_U_OFFSET";
	const char tok_map_v_offset[]       = "UVW_V_OFFSET";
	const char tok_map_u_tiling[]       = "UVW_U_TILING";
	const char tok_map_v_tiling[]       = "UVW_V_TILING";
	const char tok_map_angle[]          = "UVW_ANGLE";
	const char tok_map_blur[]           = "UVW_BLUR";
	const char tok_map_blur_offset[]    = "UVW_BLUR_OFFSET";
	const char tok_map_noise_amt[]      = "UVW_NOUSE_AMT"; // This mispelled token is valid!
	const char tok_map_noise_size[]     = "UVW_NOISE_SIZE";
	const char tok_map_noise_level[]    = "UVW_NOISE_LEVEL";
	const char tok_map_noise_phase[]    = "UVW_NOISE_PHASE";
	const char tok_map_bitmap_filter[]  = "BITMAP_FILTER";
	const char tok_map_class_bitmap[]   = "\"Bitmap\"";
	const char tok_map_class_noise[]    = "\"Noise\"";
	const char tok_map_class_mask[]     = "\"Mask\"";

	const char tok_jpeg_ext0[]          = "jpeg";
	const char tok_jpeg_ext1[]          = "jpg";
	const char tok_targa_ext[]          = "tga";

	//add by puzzy
	const char tok_rgb[]				= "\"RGB\"";//1
	const char tok_alpha[]				= "\"Alpha\"";//3
	const char tok_image[]				= "\"Image\"";//0
	const char tok_none[]				= "\"None\"";//2
	const char tok_map_channel[]		= "MAP_CHANNEL";
	const char tok_map_alphasrc[]		= "MAP_ALPHA_SOURCE"; 
	const char tok_map_mono[]			= "MAP_MONO_CHANNEL"; 
	const char tok_map_rgb[]			= "MAP_RGB_CHANNEL"; 
	const char tok_map_animspeed[]		= "MAP_PLAYBACK_RATE"; 
	const char tok_uvw_tile	[]			= "UVW_TILE";

	//aniamation control
	const char tok_timevalue []			= "TIMEVALUE";
	//linear control
	const char tok_control_pos_bezier[]	= "CONTROL_POS_BEZIER";
	const char tok_control_pos_tcb[]	= "CONTROL_POS_TCB";
	const char tok_control_pos_linear[]	= "CONTROL_POS_LINEAR";
	const char tok_control_rot_bezier[]	= "CONTROL_ROT_BEZIER";
	const char tok_control_rot_tcb[]	= "CONTROL_ROT_TCB";
	const char tok_control_rot_linear[]	= "CONTROL_ROT_LINEAR";
	const char tok_control_scale_bezier[]="CONTROL_SCALE_BEZIER";
	const char tok_control_scale_tcb[]	= "CONTROL_SCALE_TCB";
	const char tok_control_scale_linear[]="CONTROL_SCALE_LINEAR";


	//linear
	const char tok_control_pos_key[]	= "CONTROL_POS_KEY";    
	const char tok_control_rot_key[]	= "CONTROL_ROT_KEY";    
	const char tok_control_scale_key[]	= "CONTROL_SCALE_KEY";


	//spine control(TCB/Bezier/etc...)
	const char tok_control_pos_tcbkey[] = "CONTROL_TCB_POS_KEY";
	const char tok_control_rot_tcbkey[] = "CONTROL_TCB_ROT_KEY";
	const char tok_control_scale_tcbkey[] = "CONTROL_TCB_SCALE_KEY";

	const char tok_control_pos_bezierkey[] = "CONTROL_BEZIER_POS_KEY";
	const char tok_control_scale_bezierkey[] = "CONTROL_BEZIER_SCALE_KEY";

	const char tok_scene_tickperfrm[]		= "SCENE_TICKSPERFRAME";
	const char tok_scene_firstfrm[]			= "SCENE_FIRSTFRAME";
	const char tok_scene_lastfrm[]			= "SCENE_LASTFRAME";
	const char tok_scene_framespeed[]		= "SCENE_FRAMESPEED";

	const char tok_tm_scale[]				= "TM_SCALE";
	const char tok_tm_pos[]					= "TM_POS";
	const char tok_tm_rotaxis[]				= "TM_ROTAXIS";
	const char tok_tm_rotangle[]			= "TM_ROTANGLE";

	const char tok_camera[]					= "CAMERAOBJECT";
	const char tok_cam_settings[]			= "CAMERA_SETTINGS";
	const char tok_cam_animation[]			= "CAMERA_ANIMATION";
	const char tok_cam_fov[]				= "CAMERA_FOV";
	const char tok_cam_far[]				= "CAMERA_FAR";
	const char tok_cam_near[]				= "CAMERA_NEAR";
	const char tok_cam_tdist[]				= "CAMERA_TDIST";
	//---------------------------------------------------------------





	// function decl
	bool ase_gettoken(const char * stream, char * token);
	void ase_ignore(const char* stream);
	int ase_parse(const char * stream, model * m);
	int ase_geomobject(const char * stream, model * m);
	int ase_passthru(const char * stream, model * m);
	int ase_tmobject(const char * stream, model * m, geomobj * geom);

	int ase_patchmesh(const char * stream, geomobj * geom);
	int ase_patchmesh_vertex_list(const char * stream, geomobj * geom);
	int ase_patchmesh_vector_list(const char * stream, geomobj * geom);
	int ase_patchmesh_edge_list(const char * stream, geomobj * geom);
	int ase_patchmesh_patch_list(const char * stream, geomobj * geom);
	int ase_patchmesh_tvchannel_list(const char * stream, geomobj * geom);
	int ase_patchmesh_tvchannel(const char * stream, geomobj * geom, int tvchannel_idx);
	int ase_patchmesh_tvpatch_list(const char * stream, geomobj * geom, int tvchannel_idx);
	int ase_patchmesh_tvvert_list(const char * stream, geomobj * geom, int tvchannel_idx);

	int ase_mesh(const char * stream, geomobj * geom);
	int ase_mesh_anim(const char * stream, geomobj * geom);
	int ase_tm_animation(const char * stream,model* localmodel, geomobj * geom);
	int ase_rottrack(const char * stream,animdata * anim);
	int ase_postrack(const char * stream,animdata * anim);
	int ase_scaletrack(const char * stream,animdata * anim);
	int ase_vertex_weight(const char * stream, model * m, geomobj * geom);
	int ase_texturevertexlist(const char * stream, geomobj * geom);
	int ase_texturevertexlist_idx(const char * stream, geomobj * geom,int tc_idx,unsigned int number_of_elem); //by sssa2000 090924

	int ase_vertexlist(const char * stream, geomobj * geom);
	int ase_facelist(const char * stream, geomobj * geom);
	int ase_facemaplist(const char * stream, geomobj * geom);
	int ase_facemapvert(const char * stream, geomobj * geom, int index);
	int ase_normals(const char * stream, geomobj * geom);
	int ase_tangents(const char * stream, geomobj * geom);
	int ase_texturefacelist(const char * stream, geomobj * geom, int mapIdx);
	int ase_mapchannel(const char* stream,geomobj* geom,int mapIdx); //modify by sssa2000 090925
	int ase_colorfacelist(const char* stream, geomobj *geom);//add by puzzy,2001-11-6
	int ase_colorvertexlist(const char* stream, geomobj *geom);//add by puzzy,2001-11-6
	int ase_skip(const char * stream);
	int ase_material_list(const char * stream, model * m);
	int ase_material(const char * stream, matobj * mat);
	int ase_map(const char * stream, mapobj * map);

	//--ase's light info
	int ase_lightobject(const char * stream, model* m);

	int ase_gettm(const char * stream, model * m,float* matrix,float* pos,float* rot,float *scl,char* nodename);
	int ase_lightsettings(const char *stream, lightobj* light);
	//---ase 's strip
	int ase_stripobject(const char *stream, geomobj* geom);
	int ase_stripVerts(const char* stream, stripobj* strip,t_strip_mat* stripmat);
	int ase_submatstrip(const char* stream,stripobj* strip);
	int ase_getstrips(const char* stream, t_strip_mat* stripmat,int nStripMat);
	int ase_readstrips(const char* stream,t_strip_mat* stripmat,int nStripMat);
	void ase_param_strip(const char* token,t_strip* str);

	// by lance
	int ase_getlods(const char* stream, t_strip_mat* stripmat,int nStripMat);
	int ase_getlodstrips(const char* stream, strip_array* striparray,int nStripMat);

	//---ase 's level of detail, add by lance AGAIN at 2003-12-18
	int ase_lodobject(const char* stream, t_strip_mat* stripmat);

	void load_mat(model * m, matobj * mat, const char * datapath);
	void load_map(model * m, map_array * marray, const char * datapath);
	int load_map_file(const char * filename, const char * datapath, texdata * txdata);

	void convert_world_to_local(model * m);
	void compute_local_tm(geomobj * geom);

	//add by puzzy
	int ase_poskey(const char * stream,animdata * anim);
	int ase_rotkey(const char * stream,animdata * anim);
	int ase_sclkey(const char * stream,animdata * anim);
	int ase_tcbposkey(const char * stream,animdata * anim);
	int ase_tcbrotkey(const char * stream,animdata * anim);
	int ase_tcbsclkey(const char * stream,animdata * anim);
	int ase_bezierposkey(const char * stream,animdata * anim);
	int ase_beziersclkey(const char * stream,animdata * anim);
	int ase_scene(const char* stream,model*m);

	int ase_cameraobject(const char * stream, model* m);
	int ase_camsettings(const char* stream,camobj* cam);
	int ase_camanimation(const char* stream,camobj* cam);
	int ase_camani_setting(const char* stream,camobj* cam);

	void findparent(model* m);

	//static float secperfrm;
	//static model* localmodel;

	// implementation
	texdata::~texdata()
	{
		if (pixels)
		{
			delete [] pixels;
			pixels = NULL;
		}

		if (name)
			delete [] name;
		name = 0;
	}

	animdata::~animdata()
	{

	}



	mapobj::~mapobj()
	{
		map_it it = map_generic.begin();
		while (it != map_generic.end())
		{
			delete *it;
			++it;
		}
		if (name)
			delete [] name;
		name = 0;
		if (classname)
			delete [] classname;
		classname = 0;
		if (bitmap)
			delete [] bitmap;
		bitmap = 0;
		if (map_type)
			delete [] map_type;
		map_type = 0;
		if (bitmap_filter)
			delete [] bitmap_filter;
		bitmap_filter = 0;
	}

	matobj::~matobj()
	{
		map_it it = map_generic.begin();
		while (it != map_generic.end())
		{
			delete *it;
			++it;
		}

		it = map_diffuse.begin();
		while (it != map_diffuse.end())
		{
			delete *it;
			++it;
		}

		it = map_specular.begin();
		while (it != map_specular.end())
		{
			delete *it;
			++it;
		}

		it = map_bump.begin();
		while (it != map_bump.end())
		{
			delete *it;
			++it;
		}

		it = map_selfillum.begin();
		while (it != map_selfillum.end())
		{
			delete *it;
			++it;
		}

		it = map_reflect.begin();
		while (it != map_reflect.end())
		{
			delete *it;
			++it;
		}

		it = map_shine.begin();
		while (it != map_shine.end())
		{
			delete *it;
			++it;
		}

		it = map_shinestrength.begin();
		while (it != map_shinestrength.end())
		{
			delete *it;
			++it;
		}

		it = map_opacity.begin();
		while (it != map_opacity.end())
		{
			delete *it;
			++it;
		}

		it = map_refract.begin();
		while (it != map_refract.end())
		{
			delete *it;
			++it;
		}

		it = map_ambient.begin();
		while (it != map_ambient.end())
		{
			delete *it;
			++it;
		}

		it = map_filtercolor.begin();
		while (it != map_filtercolor.end())
		{
			delete *it;
			++it;
		}

		mat_it mit = submat.begin();
		while (mit != submat.end())
		{
			delete *mit;
			++mit;
		}
		if (name)
			delete [] name;
		name = 0;
		if (classname)
			delete [] classname;
		classname = 0;
		if (shader)
			delete [] shader;
		shader = 0;
		if (xp_type)
			delete [] xp_type;
		xp_type = 0;
	}


	geomobj::geomobj() : parent(0), numn(0), numTn(0), numv(0), numf(0), numtf(0), numc(0), numt(0), anim(0), fmapv(0), matidx(-1), numbv(0), name(0)
	{   
		tm[0] = 1.0f;tm[1] = 0.0f;tm[2] = 0.0f;tm[3] = 0.0f;
		tm[4] = 0.0f;tm[5] = 1.0f;tm[6] = 0.0f;tm[7] = 0.0f;
		tm[8] = 0.0f;tm[9] = 0.0f;tm[10] = 1.0f;tm[11] = 0.0f;
		tm[12] = 0.0f;tm[13] = 0.0f;tm[14] = 0.0f;tm[15] = 1.0f;
		strip = 0;
		numf = numcf = 0;
		numtf = 0;
		numtf1 = 0;
		tf = 0;
		tf1 = 0;
		numtvchannels = 0;
		anim = 0;
		strip=0;
		classname = 0;
		parentname = 0;

		//add by sssa2000 090925
		t=0;
		t1=0;
		t2=0;
		t3=0;

		numt1=0;
		numt2=0;
		numt3=0;
	}

	geomobj::~geomobj() 
	{
		if (numv)
		{
			delete [] v;
			numv = 0;
		}

		if (numt)
		{
			if (t)
			{
				delete [] t;
				t=0;
			}

			if (t1)
			{
				delete [] t1;
				t1=0;
			}

			if (t2)
			{
				delete [] t2;
				t2=0;
			}

			if (t3)
			{
				delete [] t3;
				t3=0;
			}

			numt = 0;

		}

		if (numf)
		{
			delete [] f;
			delete [] smg;
			delete [] fsubmat;
			numf = 0;
		}

		if (numn)
		{
			delete [] fn;
			delete [] fvn;
			delete [] n;
			numn = 0;
		}

		if (numTn)
		{
			delete [] fvTn;
			delete [] Tn;
			numTn = 0;
		}

		if (numc)
		{
			delete [] c;
			numc = 0;
		}

		if (numcf)
		{
			delete []cf;
			numcf = 0;
		}
		if (numtf)
		{
			delete [] tf;
			numtf = 0;
		}

		if (numtf1)
		{
			delete [] tf1;
			numtf1 = 0;
		}

		if (fmapv)
		{
			delete [] fmapv;
			fmapv = NULL;
		}

		if (anim)
		{
			delete anim;
			anim = NULL;
		}

		if (numbv)
		{
			delete [] bv;
			delete [] bmatref;
			delete [] vbv;
			numbv = 0;
		}


		if (strip) delete strip;

		if (name)
			delete [] name;

		if (classname)
			delete []classname;

		if (parentname)
			delete []parentname;

		if (anim_mesh.size())
			for (int i=0;i<anim_mesh.size();++i)
				delete anim_mesh[i];

		name = 0;
	}

	int geomobj::CreateTc(int idx,unsigned int number_of_elem)
	{
		float* tc_array[]={t,t1,t2,t3};
		unsigned int* tc_num_array[]={&numt,&numt1,&numt2,&numt3};

		float** tc_address_array[]={&t,&t1,&t2,&t3};

		if (!tc_array[idx])
		{
			*(tc_address_array[idx])=new float[number_of_elem * 3];
			*(tc_num_array[idx]) = number_of_elem;
			return 1;
		}
		return 0;

	}
	model::~model()
	{
		geom_it git = geom.begin();
		while (git != geom.end())
		{
			delete *git;
			++git;
		}

		mat_it mit = mat.begin();
		while (mit != mat.end())
		{
			delete *mit;
			++mit;
		}

		tex_it tit = tex.begin();
		while (tit != tex.end())
		{
			delete (*tit).second;
			++tit;
		}

		light_it lit = light.begin();
		while(lit!=light.end())
		{
			delete (*lit);
			++lit;
		}

		for (int i=0;i<cam.size();++i)
			delete (camobj*)(cam[i]);


		if (name)
			delete [] name;
		name = 0;
	};

	//bool load_tex(const char * datapath, model * m)
	//{
	//    geom_it it = m->geom.begin();

	//    while(it != m->geom.end())
	//    {
	//        int matidx = (*it)->matidx;
	//        if (matidx >= 0)
	//            load_mat( m, m->mat[matidx], datapath);
	//        ++it;
	//    }
	//    return true;
	//}

	//void load_mat(model * m, matobj * mat, const char * datapath)
	//{
	//    load_map( m, &mat->map_ambient ,datapath);
	//    load_map( m, &mat->map_generic ,datapath);
	//    load_map( m, &mat->map_diffuse ,datapath);
	//    load_map( m, &mat->map_specular ,datapath);
	//    load_map( m, &mat->map_bump ,datapath);
	//    load_map( m, &mat->map_selfillum ,datapath);
	//    load_map( m, &mat->map_reflect ,datapath);
	//    load_map( m, &mat->map_shine ,datapath);

	//    mat_it it = mat->submat.begin();
	//    while (it != mat->submat.end())
	//    {
	//        load_mat(m, *it, datapath);
	//        ++it;
	//    }
	//}

	/*   void load_map(model * m, map_array * marray, const char * datapath)
	{
	map_it it = marray->begin();
	while (it != marray->end())
	{
	mapobj * map = *it;
	if (!strcmp(map->classname,tok_map_class_bitmap))
	{
	tex_it txit = m->tex.find(map->bitmap);
	if (txit == m->tex.end())
	{
	texdata * txdata = new texdata;
	if (load_map_file(map->bitmap, datapath, txdata) == 0)
	{
	m->tex.insert(tex_pair(map->bitmap,txdata));
	}
	}
	}
	else if (!strcmp(map->classname,tok_map_class_mask))
	{
	map_it maskit = map->map_generic.begin();
	while (maskit != map->map_generic.end())
	{
	mapobj * maskmap = *maskit;
	tex_it txit = m->tex.find(maskmap->bitmap);
	if (txit == m->tex.end())
	{
	texdata * txdata = new texdata;
	if (load_map_file(maskmap->bitmap, datapath, txdata) == 0)
	{
	m->tex.insert(tex_pair(maskmap->bitmap,txdata));
	}
	}
	++maskit;
	}
	}
	load_map( m, &map->map_generic ,datapath);
	++it;
	}
	}*/

	//int load_map_file(const char * filename, const char * datapath, texdata * txdata)
	//{
	//    int result = 0;
	//    char fullpath[MAX_PATH];

	//    // extract the file extension
	//    char * ext = strrchr(filename,'.');
	//    if (ext == NULL)
	//        result = 1;
	//    else
	//    {
	//        ext++;

	//        sprintf(fullpath,"%s%s",datapath,filename);
	//        // do case insensitive string compares
	//        if (!_stricmp(ext, tok_jpeg_ext0) || !_stricmp(ext, tok_jpeg_ext1))
	//        {
	//            if (result = jpeg::read(fullpath,&txdata->width, &txdata->height, &txdata->pixels, &txdata->components))
	//                // Sometimes the filename contains already the full pathname!
	//                result = jpeg::read(filename,&txdata->width, &txdata->height, &txdata->pixels, &txdata->components);
	//            if (result)
	//            {
	//                char errstr[256];
	//                sprintf(errstr,"load_map_file::jpeg: Failed to load - %s or %s  - with datapath %s\n", filename, fullpath, datapath );
	//                OutputDebugString(errstr);
	//            }
	//        }
	//        else if (!_stricmp(ext, tok_targa_ext))
	//        {
	//            tga::tgaImage * image = tga::read(fullpath);
	//            if (image == NULL)
	//                image = tga::read(filename);
	//            if (image)
	//            {
	//                txdata->width = image->width;
	//                txdata->height = image->height;
	//                txdata->pixels = image->pixels;
	//                txdata->pixelfmt = image->format;
	//                txdata->components = image->components;
	//                free(image);
	//            }
	//            else
	//                result = 1;
	//            if (result)
	//            {
	//                char errstr[256];
	//                sprintf(errstr,"load_map_file::tga: Failed to load - %s or %s  - with datapath %s\n", filename, fullpath, datapath );
	//                OutputDebugString(errstr);
	//            }
	//        }
	//        else
	//        {
	//            char errstr[256];
	//            sprintf(errstr,"load_map_file: Unsupported file format .- %s for %s\n", ext, filename);
	//            OutputDebugString(errstr);
	//            result = 1;
	//        }
	//    }

	//    if (result)
	//    {
	//        char errstr[256];
	//        sprintf(errstr,"load_map_file: Failed to load. %s \n", filename);
	//        OutputDebugString(errstr);
	//    }
	//    else
	//    {
	//        txdata->name = new char[strlen(filename) + 1];
	//        strcpy(txdata->name, filename);
	//    }


	//    return result;
	//}


	model* read_h3dfile(const char* name);
	//static int worldVert ;

	model* load(const char* filename,float scale,int bWorld)
	{
		//worldVert = bWorld;
		//return load(name,scale);

		int     result(0);
		//deleted by yanshuai
		/*HANDLE  hMapping, hFile;
		char *  fileBytes = NULL;
		DWORD   fileSizeHigh  = 0;
		DWORD   fileSizeLow  = 0;
		int     fileSize;*/

		//      hFile =
		//      CreateFile(filename,      // Open our filename
		//          GENERIC_READ,          // Open it readonly
		//          0,                     // don't share
		//          NULL,                  // No security for child procs
		//          OPEN_EXISTING,         // Open the existing file
		//          FILE_ATTRIBUTE_NORMAL |    // It's a normal file
		//          FILE_FLAG_SEQUENTIAL_SCAN, // that we read sequentially
		//          NULL);                     // no template

		//      if (hFile == NULL || hFile == (HANDLE)0xffffffff)
		//          return NULL;

		//      fileSizeLow = GetFileSize(hFile, &fileSizeHigh);
		//      if (fileSizeLow == 0xffffffff)
		//{
		//          return NULL;
		//}

		//      // If there is no such mapper, create one
		//      hMapping = OpenFileMapping( FILE_MAP_COPY, 0, NV_ASE_MAPPER_NAME);

		//      if (hMapping == NULL)
		//      {
		//          hMapping = ::CreateFileMapping(hFile,      // handle from above
		//		            NULL,            // No security for children
		//		            PAGE_READONLY,  // Write to a copy of the file
		//		            0,               // High and low bits of max
		//		            0,               // size == 0 -> whole file
		//		            NV_ASE_MAPPER_NAME); // mapper name

		//          // Did we get anything?  If not, error
		//          if (hMapping == NULL)
		//          {
		//              CloseHandle (hMapping);
		//              return NULL;
		//          }
		//      }

		//      // Finally, map the file into our local address space
		//      fileBytes = (char *)
		//      MapViewOfFile(hMapping,      // the file mapper
		//          FILE_MAP_READ,    // Copy so that we can null terminate
		//          0,                // offset high bit
		//          0,                // offset low bit
		//          0);               // # to map (0==all)
		//      assert(fileSizeHigh == 0);
		//      //fileSize = (fileSizeHigh<<32) + fileSizeLow;
		//fileSize = fileSizeLow;

		//////////////////////////////////////////////////////////////////////////
		//add by yanshuai
		struct stat st;
		stat(filename, &st);
		int size = st.st_size;
		FILE* file = fopen(filename, "rb");
		if (!file)
		{
			return NULL;
		}
		char* buff = new char[size + 1];
		fread(buff, size, 1, file);
		buff[size] = '\0';
		fclose(file);
		//////////////////////////////////////////////////////////////////////////
		g_stream_max_cnt = size;
		g_stream_cnt = 0;
		g_scale_hack = scale;
		model * m = new model;
		//localmodel = m;
		result = ase_parse(buff,m);
		delete []buff;

		// Done with the file mapping
		//deleted by yanshuai
		/*if(!UnmapViewOfFile(fileBytes)   ||
		!CloseHandle    (hMapping) ||
		!CloseHandle    (hFile))
		result = 1;*/

		if (result == 0)
		{
			if (!bWorld)
				convert_world_to_local(m);
			else
			{
				// delete by lance 2003-12-25 christmas
				// normals are exported in world coordinates now
				//convert_normals_to_world(m);
				//worldVert=0;//reset
			}


			geom_it it = m->root_geom.begin();
			while(it != m->root_geom.end())
			{
				compute_local_tm(*it);
				++it;
			}
		}
		else
		{
			delete m;
			m = NULL;
		}

		findparent(m);
		return m;
	}

	void convert_normals_to_world(model* m)
	{
		int i,_3;
		geomobj* geom = NULL;
		glh::vec3f norm;

		for (int n=0;n<m->geom.size();++n)
		{
			geom = m->geom[n];

			glh::matrix4f mat(geom->tm);
			glh::matrix4f inv = mat.inverse();
			inv  = inv.transpose();

			if (!geom->strip)
			{
				for (i=0;i<geom->numn;++i)
				{
					_3 =  i*3;
					glh::vec3f vec(&geom->n[_3]);
					inv.mult_dir_matrix(vec,norm);
					norm.normalize();
					geom->n[_3] = norm[0];
					geom->n[_3+1] = norm[1];
					geom->n[_3+2] = norm[2];
				}
			}
			else
			{
				int i_times_3;
				for (i=0;i<geom->strip->strip_mat.size();++i)
				{
					t_strip_mat* stripmat = geom->strip->strip_mat[i];

					for (int j=0;j<stripmat->numVert;++j)
					{
						i_times_3 = j * 3;
						glh::vec3f vec(&stripmat->n[i_times_3]);
						inv.mult_matrix_dir(vec);
						vec.normalize();
						stripmat->n[i_times_3] = vec[0];
						stripmat->n[i_times_3+1] = vec[1];
						stripmat->n[i_times_3+2] = vec[2];
					}
				}

			}
		}
	}

	void findparent(model* m)
	{
		if (!m->geom.size()) return;

		for (size_t i=0;i<m->geom.size();++i)
		{
			geomobj* obj = m->geom[i];
			if (!obj->name || !obj->name[0]) continue;

			for (size_t j=i+1;j<m->geom.size();++j)
			{
				geomobj* obj1 = m->geom[j];
				if (!obj1->name || !obj1->name[0]) continue;

				//same name obj
				if ( !strcmp(obj->name,obj1->name)) continue;

				if (obj1->parentname)
					if (!strcmp(obj->name,obj1->parentname))
						obj1->parent = obj;

				if (obj->parentname)
					if (!strcmp(obj->parentname ,obj1->name))
						obj->parent = obj1;
			}
		}
	}

	model * load(const char * filename, float scale)
	{
		//以前的逻辑是：无论load传入bWorld是1还是0，该变量始终会被清0。所以当调用不带bWorld参数的接口时，等价于传入0
		return load(filename,scale,0);
	}

	model * load(const char * buf, unsigned int bufsize, float scale,int bWorld)
	{
		int     result(0);

		g_stream_max_cnt = bufsize;
		g_stream_cnt = 0;
		g_scale_hack = scale;
		model * m = new model;
		//localmodel = m;
		result = ase_parse(buf,m);

		// Done with the file mapping
		//deleted by yanshuai
		/*if(!UnmapViewOfFile(fileBytes)   ||
		!CloseHandle    (hMapping) ||
		!CloseHandle    (hFile))
		result = 1;*/

		if (result == 0)
		{
			if (!bWorld)
				convert_world_to_local(m);
			else
			{
				// delete by lance 2003-12-25 christmas
				// normals are exported in world coordinates now
				//convert_normals_to_world(m);
				//worldVert=0;//reset
			}


			geom_it it = m->root_geom.begin();
			while(it != m->root_geom.end())
			{
				compute_local_tm(*it);
				++it;
			}
		}
		else
		{
			delete m;
			m = NULL;
		}

		findparent(m);
		return m;








		//int result(0);
		//g_stream_max_cnt = bufsize;
		//g_stream_cnt = 0;
		//g_scale_hack = scale;
		//model * m = new model;
		//localmodel = m;

		//result = ase_parse(buf,m);

		//if (result == 0)
		//{
		//	geom_it rit;
		//	geom_it git = m->geom.begin();
		//	while (git != m->geom.end())
		//	{
		//		// In case we have a group as parent
		//		if ((*git)->parent == NULL)
		//		{
		//			// Lets amke sure it hasn't already been added 
		//			bool found = false;
		//			rit = m->root_geom.begin();
		//			while (rit != m->root_geom.end() && found)
		//			{
		//				if (!strcmp((*rit)->name, (*git)->name))
		//					found = true;
		//				++rit;
		//			}
		//			if (found == false)
		//				m->root_geom.push_back(*git);
		//		}
		//		++git;
		//	}

		//	convert_world_to_local(m);

		//	rit = m->root_geom.begin();
		//	while(rit != m->root_geom.end())
		//	{
		//		compute_local_tm(*rit);
		//		++rit;
		//	}

		//	// Put the transparent objectsat the end
		//	// Not a solution just a hack for the silly renderer
		//	geom_array transgeom;

		//	git = m->geom.begin();
		//	while (git != m->geom.end())
		//	{
		//		matobj * mat = NULL;
		//		if ((*git)->matidx >= 0)
		//		{
		//			mat = m->mat[(*git)->matidx];
		//			if (mat->transparency > 0.0f)
		//			{
		//				transgeom.push_back(*git);
		//				m->geom.erase(git);
		//			}
		//		}
		//		++git;
		//	}

		//	geom_it tit = transgeom.begin();
		//	while (tit != transgeom.end())
		//	{
		//		m->geom.push_back(*tit);
		//		++tit;
		//	}        

		//}
		//else
		//{
		//	delete m;
		//	m = NULL;
		//}

		//return m;
	}


	void compute_local_tm(geomobj * geom)
	{
		if (geom->parent)
		{
			glh::matrix4f matparent(geom->parent->tm);
			matparent = matparent.inverse();
			glh::matrix4f mat(geom->tm);
			mat = matparent * mat;
			memcpy(geom->rtm,mat.get_value(),sizeof(float) * 16);
		}

		geom_it it = geom->children.begin();
		while (it != geom->children.end())
		{
			compute_local_tm(*it);
			++it;
		}
	}


	void convert_world_to_local(model * m)
	{
		int i;
		int i_times_3;
		geomobj * geom = NULL;
		for (int n=0;n<m->geom.size();++n)
		{
			geom = m->geom[n];
			glh::matrix4f mat(geom->tm);
			glh::matrix4f invmat = mat.inverse();
			for (i = 0; i < geom->numv; ++i)
			{
				i_times_3 = i * 3;
				glh::vec3f vec(&geom->v[i_times_3]);
				invmat.mult_matrix_vec(vec);
				geom->v[i_times_3] = vec[0];
				geom->v[i_times_3 + 1] = vec[1];
				geom->v[i_times_3 + 2] = vec[2];
			}
			if (geom->anim_mesh.size())
			{
				for (int j=0;j<geom->anim_mesh.size();++j)
				{
					geomobj* ag = geom->anim_mesh[j];
					for (int k = 0; k < ag->numv; ++k)
					{
						i_times_3 = k * 3;
						glh::vec3f vec(&ag->v[i_times_3]);
						invmat.mult_matrix_vec(vec);
						ag->v[i_times_3] = vec[0];
						ag->v[i_times_3 + 1] = vec[1];
						ag->v[i_times_3 + 2] = vec[2];
					}

					if (ag->strip)
					{
						for (i=0;i<ag->strip->strip_mat.size();++i)
						{
							t_strip_mat* stripmat = ag->strip->strip_mat[i];

							for (int j=0;j<stripmat->numVert;++j)
							{
								i_times_3 = j * 3;
								glh::vec3f vec(&stripmat->v[i_times_3]);
								invmat.mult_matrix_vec(vec);
								stripmat->v[i_times_3] = vec[0];
								stripmat->v[i_times_3+1] = vec[1];
								stripmat->v[i_times_3+2] = vec[2];
							}
						}//translate strip's vertex
					}
				}
			}

			//convert the strip verts
			if (geom->strip)
			{
				for (i=0;i<geom->strip->strip_mat.size();++i)
				{
					t_strip_mat* stripmat = geom->strip->strip_mat[i];

					for (int j=0;j<stripmat->numVert;++j)
					{
						i_times_3 = j * 3;
						glh::vec3f vec(&stripmat->v[i_times_3]);
						invmat.mult_matrix_vec(vec);
						stripmat->v[i_times_3] = vec[0];
						stripmat->v[i_times_3+1] = vec[1];
						stripmat->v[i_times_3+2] = vec[2];
					}
				}
			}

			// According to the MAX SDK Doc, normals are in object space
			/*
			for (i = 0; i < geom->numn; ++i)
			{
			i_times_3 = i * 3;
			glh::vec3f vec(&geom->n[i_times_3]);
			invmat.mult_matrix_dir(vec);
			geom->n[i_times_3] = vec[0];
			geom->n[i_times_3 + 1] = vec[1];
			geom->n[i_times_3 + 2] = vec[2];
			}
			*/

		}
	}

	void ase_ignore(const char * stream)
	{
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
			else if (!strcmp(g_leftover,tok_begin))		
			{
				ase_ignore(stream);
			}
		}
	}

	int ase_parse(const char * stream, model * m)
	{
		int  result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_group))
			{
				result |= ase_passthru(stream,m);
			}
			else if (!strcmp(g_keyword,tok_geomobject))
			{
				result |= ase_geomobject(stream,m);
			}
			else if (!strcmp(g_keyword,tok_helperobject))
			{
				result |= ase_geomobject(stream,m);
			}
			else if (!strcmp(g_keyword,tok_material_list))
			{
				result |= ase_material_list(stream,m);
			}
			else if (!strcmp(g_keyword,tok_light))
			{
				result |= ase_lightobject(stream,m);
			}
			else if (!strcmp(g_keyword,tok_camera))
			{
				result |= ase_cameraobject(stream,m);
			}
			else if (!strcmp(g_keyword,tok_scene))
			{
				result |= ase_scene(stream,m);
			}
		}
		return result;
	}

	bool ase_gettoken(const char * stream, char * token)
	{
		int cnt;

		while (g_stream_cnt < g_stream_max_cnt&&((!isprint(stream[g_stream_cnt])) || stream[g_stream_cnt] == ' '))
			g_stream_cnt++;

		if (g_stream_cnt>=g_stream_max_cnt)
			return false;

		switch(stream[g_stream_cnt]) 
		{
		case '*':
			g_stream_cnt++;
			cnt = g_stream_cnt;
     		while (cnt < g_stream_max_cnt&&stream[cnt] != '\n')
				cnt++;

			if (cnt>=g_stream_max_cnt)
				return false;

			memcpy(token,stream + g_stream_cnt, cnt - g_stream_cnt);
			token[ cnt - g_stream_cnt - 1] = '\0';
			g_stream_cnt = cnt;
			return true;
		case '}':
			while (g_stream_cnt < g_stream_max_cnt&&stream[g_stream_cnt] != '\n')
				g_stream_cnt++;

			if (g_stream_cnt>=g_stream_max_cnt)
				return false;

			token[0] = '}';
			token[1] = '\0';
			return true;
		default:
			while (g_stream_cnt < g_stream_max_cnt&&stream[g_stream_cnt] != '\n')
				g_stream_cnt++;
			/* eat up rest of line */
			if (g_stream_cnt>=g_stream_max_cnt)
				return false;
			break;
		}
		return false;
	}

	int ase_scene(const char* stream,model*m)
	{
		int result = 0;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_scene_firstfrm))
			{
				sscanf(g_leftover, "%d",&m->firstfrm);
			}
			else if (!strcmp(g_keyword,tok_scene_lastfrm))
			{
				sscanf(g_leftover, "%d",&m->lastfrm);
			}
			else if (!strcmp(g_keyword,tok_scene_framespeed))
			{
				sscanf(g_leftover, "%d",&m->framespeed);
			}
			else if (!strcmp(g_keyword,tok_scene_tickperfrm))
			{
				sscanf(g_leftover, "%d",&m->tickperfrm);
			}

			else if (!strcmp(g_keyword,"}"))
			{

				break;
			}
		}
		return result;
	}


	int ase_material_list(const char * stream, model * m)
	{
		int result = 0;
		int num_materials = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_material_count))
			{
				sscanf(g_leftover, "%d",&num_materials);
			}
			else if (!strcmp(g_keyword,tok_material))
			{
				matobj * mat = new matobj;
				result |= ase_material(stream , mat);
				m->mat.push_back(mat);
			}
			else if (!strcmp(g_keyword,"}"))
			{
				assert(m->mat.size() == num_materials);
				break;
			}
		}

		return result;
	}

	int ase_material(const char * stream, matobj * mat)
	{
		int result = 0;
		char buf[256];

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_material_name))
			{
				const char * pStart = strchr(g_token, '\"');
				const char * pEnd = pStart + 1;
				const char * pit = pEnd;
				while (*pit != '\0' )
				{
					if (*pit == '"')
						pEnd = pit;
					++pit;
				}
				int size = pEnd - pStart + 1;
				memcpy(buf,pStart,size);
				buf[size-1] = '\0';

				mat->name = new char[strlen(buf) + 1];
				strcpy(mat->name,buf+1);
			}
			else if (!strcmp(g_keyword,tok_material_class))
			{
				const char * pStart = strchr(g_token, '\"');
				const char * pEnd = pStart + 1;
				const char * pit = pEnd;
				while (*pit != '\0' )
				{
					if (*pit == '"')
						pEnd = pit;
					++pit;
				}
				memcpy(buf,pStart,pEnd - pStart + 1);
				buf[pEnd - pStart + 1] = '\0';

				mat->classname = new char[strlen(buf) + 1];
				strcpy(mat->classname,buf);
			}
			else if (!strcmp(g_keyword,tok_material_ambient))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &mat->ambient[0],&mat->ambient[1],&mat->ambient[2]);
			}
			else if (!strcmp(g_keyword,tok_material_diffuse))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &mat->diffuse[0],&mat->diffuse[1],&mat->diffuse[2]);
			}
			else if (!strcmp(g_keyword,tok_material_specular))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &mat->specular[0],&mat->specular[1],&mat->specular[2]);
			}
			else if (!strcmp(g_keyword,tok_material_shine))
			{
				sscanf(g_token, "%s %f",g_keyword, &mat->shine);
			}
			else if (!strcmp(g_keyword,tok_material_shinestrength))
			{
				sscanf(g_token, "%s %f",g_keyword, &mat->shinestrength);
			}
			else if (!strcmp(g_keyword,tok_material_transparency))
			{
				sscanf(g_token, "%s %f",g_keyword, &mat->transparency);
			}
			else if (!strcmp(g_keyword,tok_material_wiresize))
			{
				sscanf(g_token, "%s %f",g_keyword, &mat->wiresize);
			}
			else if (!strcmp(g_keyword,tok_material_shading))
			{
				mat->shader = new char[strlen(g_leftover) + 1];
				strcpy(mat->shader, g_leftover);
			}
			else if (!strcmp(g_keyword,tok_material_xp_falloff))
			{
				sscanf(g_token, "%s %f",g_keyword, &mat->xp_falloff);
			}
			else if (!strcmp(g_keyword,tok_material_selfillum))
			{
				sscanf(g_token, "%s %f",g_keyword, &mat->selfillum);
			}
			else if (!strcmp(g_keyword,tok_material_twosided))
			{
				mat->twosided = true;
			}
			else if (!strcmp(g_keyword,tok_material_falloff))
			{
				if (!strcmp(g_leftover,"In"))
					mat->falloff = true; // In
				else
					mat->falloff = false; // Out
			}
			else if (!strcmp(g_keyword,tok_material_xp_type))
			{
				mat->xp_type = new char[strlen(g_leftover) + 1];
				strcpy(mat->xp_type, g_leftover);
			}
			else if (!strcmp(g_keyword,tok_map_diffuse))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_diffuse.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_map_ambient))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_ambient.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_map_generic))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_generic.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_map_specular))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_specular.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_map_shine))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_shine.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_map_shinestrength))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_shinestrength.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_map_bump))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_bump.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_map_selfillum))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_selfillum.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_map_reflect))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_reflect.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_map_refract))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_refract.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_map_filtercolor))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_filtercolor.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_map_opacity))
			{
				mapobj * map = new mapobj;
				result |= ase_map(stream, map);
				mat->map_opacity.push_back(map);
			}
			else if (!strcmp(g_keyword,tok_submaterial))
			{
				matobj * submat = new matobj;
				result |= ase_material(stream, submat);
				mat->submat.push_back(submat);
			}
			else if (!strcmp(g_keyword,"}"))
			{
				mat->diffuse[3] = 1.0 - mat->transparency;
				mat->specular[3] = 1.0;// - mat->transparency;
				break;
			}
		}
		return result;
	}


	int ase_map(const char * stream, mapobj * map)
	{
		char buf[256];
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_map_name))
			{
				const char * pStart = strchr(g_token, '\"');
				const char * pEnd = pStart + 1;
				const char * pit = pEnd;
				while (*pit != '\0' )
				{
					if (*pit == '"')
						pEnd = pit;
					++pit;
				}
				memcpy(buf,pStart,pEnd - pStart + 1);
				buf[pEnd - pStart + 1] = '\0';

				map->name = new char[strlen(buf) + 1];
				strcpy(map->name,buf);

			}
			else if (!strcmp(g_keyword,tok_map_class))
			{
				const char * pStart = strchr(g_token, '\"');
				const char * pEnd = pStart + 1;
				const char * pit = pEnd;
				while (*pit != '\0' )
				{
					if (*pit == '"')
						pEnd = pit;
					++pit;
				}
				memcpy(buf,pStart,pEnd - pStart + 1);
				buf[pEnd - pStart + 1] = '\0';

				map->classname = new char[strlen(buf) + 1];
				strcpy(map->classname,buf);
			}
			else if (!strcmp(g_keyword,tok_map_subno))
			{
				sscanf(g_token, "%s %d",g_keyword, &map->subno);
			}
			else if (!strcmp(g_keyword,tok_map_amount))
			{ 
				sscanf(g_token, "%s %f",g_keyword, &map->amount);
			}
			else if (!strcmp(g_keyword,tok_map_bitmap))
			{
				/*              char * pbuf = &g_leftover[1];
				g_leftover[strlen(g_leftover) - 1] = '\0';
				map->bitmap = new char[strlen(pbuf)+1];
				strcpy(map->bitmap,pbuf);
				*/
				static char pbuf[256];
				strcpy(pbuf,strchr(g_token,'\"')+1);
				//    const char * pStart =strchr(g_token, '\"')+1;
				const char * pEnd = pbuf + 1;
				const char * pit = pEnd;
				while (*pit != '\0' )
				{
					if (*pit == '"')
						pEnd = pit;
					++pit;
				}
				memcpy(buf,pbuf,pEnd - pbuf + 1);
				buf[pEnd - pbuf ] = '\0';

				map->bitmap = new char[strlen(buf) + 1];
				strcpy(map->bitmap,buf);
			}
			else if (!strcmp(g_keyword,tok_map_type))
			{
				map->map_type = new char[strlen(g_leftover) + 1];
				strcpy(map->map_type, g_leftover);
			}
			else if (!strcmp(g_keyword,tok_map_u_offset))
			{
				sscanf(g_token, "%s %f",g_keyword, &map->u_offset);
			}
			else if (!strcmp(g_keyword,tok_map_v_offset))
			{
				sscanf(g_token, "%s %f",g_keyword, &map->v_offset);
			}
			else if (!strcmp(g_keyword,tok_map_u_tiling))
			{
				sscanf(g_token, "%s %f",g_keyword, &map->u_tiling);
			}
			else if (!strcmp(g_keyword,tok_map_v_tiling))
			{
				sscanf(g_token, "%s %f",g_keyword, &map->v_tiling);
			}
			else if (!strcmp(g_keyword,tok_uvw_tile))
			{
				map->map_tile= atoi(g_leftover);
			}
			else if (!strcmp(g_keyword,tok_map_angle))
			{
				sscanf(g_token, "%s %f",g_keyword, &map->angle);
			}
			else if (!strcmp(g_keyword,tok_map_blur))
			{
				sscanf(g_token, "%s %f",g_keyword, &map->blur);
			}
			else if (!strcmp(g_keyword,tok_map_blur_offset))
			{
				sscanf(g_token, "%s %f",g_keyword, &map->blur_offset);
			}
			else if (!strcmp(g_keyword,tok_map_noise_amt))
			{
				sscanf(g_token, "%s %f",g_keyword, &map->noise_amt);
			}
			else if (!strcmp(g_keyword,tok_map_noise_size))
			{
				sscanf(g_token, "%s %f",g_keyword, &map->noise_size);
			}
			else if (!strcmp(g_keyword,tok_map_noise_level))
			{
				sscanf(g_token, "%s %d",g_keyword, &map->noise_level);
			}
			else if (!strcmp(g_keyword,tok_map_noise_phase))
			{
				sscanf(g_token, "%s %f",g_keyword, &map->noise_phase);
			}
			else if (!strcmp(g_keyword,tok_map_bitmap_filter))
			{
				map->bitmap_filter = new char[strlen(g_leftover) + 1];
				strcpy(map->bitmap_filter, g_leftover);
			}
			else if (!strcmp(g_keyword,tok_map_generic))
			{
				mapobj * submap = new mapobj;
				result |= ase_map(stream,submap);
				map->map_generic.push_back(submap);
			}
			//add by puzzy------
			/*
			const char tok_rgb[]				= "\"RGB\"";//1
			const char tok_alpha[]				= "\"Alpha\"";//3
			const char tok_image[]				= "\"Image\"";//0
			const char tok_none[]				= "\"None\"";//2
			const char tok_map_channel[]		= "MAP_CHANNEL";
			const char tok_map_alphasrc			= "MAP_ALPHA_SOURCE"; 
			const char tok_map_mono				= "MAP_MONO_CHANNEL"; 
			const char tok_map_rgb				= "MAP_RGB_CHANNEL"; 
			const char tok_map_animspeed		= "MAP_PLAYBACK_RATE"; 
			*/
			else if (!strcmp(g_keyword,tok_map_channel))
			{
				sscanf(g_token,"%s %d",g_keyword,&map->channel);
			}
			else if (!strcmp(g_keyword,tok_map_alphasrc))
			{
				if (!strcmp(g_leftover,tok_image))
					map->alpha_src = 0;
				else if (!strcmp(g_leftover,tok_rgb))
					map->alpha_src = 1;
				else if (!strcmp(g_leftover,tok_none))
					map->alpha_src = 2;
			}
			else if (!strcmp(g_keyword,tok_map_mono))
			{
				if (!strcmp(g_leftover,tok_rgb))
					map->mono_channel = 0;//rgb as mono
				else map->mono_channel=1;//alpha as mono
			}
			else if (!strcmp(g_keyword,tok_map_rgb))
			{
				if (!strcmp(g_leftover,tok_rgb))
					map->rgb_channel = 1;
				else map->rgb_channel=0;
			}
			else if (!strcmp(g_keyword,tok_map_animspeed))
			{
				sscanf (g_token,"%s %f",g_keyword,&map->anim_speed);
			}

			//------------------
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}

		//caculate the texture matrix
		float mat_scl[9];
		float mat_trans[9];
		float mat_rot[9];
		memset (mat_scl,0,sizeof(float)*9);
		memset (mat_trans,0,sizeof(float)*9);
		memset (mat_rot,0,sizeof(float)*9);
		mat_scl[0] = map->u_tiling;
		mat_scl[4] = map->v_tiling;
		mat_scl[8] = 1;
		mat_trans[0] = mat_trans[4] = mat_trans[8] = 1.f;
		mat_trans[6] = map->u_offset;
		mat_trans[7] = map->v_offset;
		mat_rot[0] = cosf(map->angle/180.f*3.14159f);
		mat_rot[1] = sinf(map->angle/180.f*3.14159f);
		mat_rot[3] = -sinf(map->angle/180.f*3.14159f);
		mat_rot[4] = cosf(map->angle/180.f*3.14159f);
		mat_rot[8] = 1.f;
		//map->tmat = mat_scl * mat_rot * mat_trans
		float tmp[9];
		mat33_mmult(mat_scl,mat_rot,tmp);
		mat33_mmult(tmp,mat_trans,map->tmat);

		return result;
	}

	geomobj * get_geomobj(model * m, const char * name)
	{
		geomobj * geom = NULL;
		geom_it it = m->geom.begin();
		while (geom == NULL && it != m->geom.end())
		{
			if (!strcmp((*it)->name,name))
				geom = *it;
			++it;
		}
		return geom;
	}

	int ase_geomobject(const char * stream, model * m)
	{
		char buf[256];
		int result = 0;

		geomobj * parent = NULL;
		geomobj * geom = NULL;

		while (ase_gettoken(stream, g_token))
		{
			if (strstr(g_token, "*STRIPS"))
			{
				int a = 0;
			}
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_node_name))
			{
				const char * pStart = strchr(g_token, '\"');
				const char * pEnd = strrchr(g_token, '\"');
				memcpy(buf,pStart,pEnd - pStart + 1);
				buf[pEnd - pStart + 1] = '\0';

				//deleted by puzzy, 2001-10-23
				//  geom = get_geomobj(m,buf);
				if (geom == NULL)
				{
					geom = new geomobj;
					geom->name = new char[strlen(buf) + 1];
					strcpy(geom->name,buf);
				}
			}
			else if (!strcmp(g_keyword,tok_helpclass))
			{
				const char * pStart = strchr(g_token, '\"');
				const char * pEnd = strrchr(g_token, '\"');
				memcpy(buf,pStart,pEnd - pStart + 1);
				buf[pEnd - pStart + 1] = '\0';
				if (geom->classname == NULL)
				{
					geom->classname = new char[strlen(buf)+1];
					strcpy(geom->classname,buf);
				}
			}

			else if (!strcmp(g_keyword,tok_node_parent))
			{
				const char * pStart = strchr(g_token, '\"');
				const char * pEnd = strrchr(g_token, '\"');
				memcpy(buf,pStart,pEnd - pStart + 1);
				buf[pEnd - pStart + 1] = '\0';
				//deleted by puzzy, 2001-10-23
				//    parent = get_geomobj(m,buf);
				if (geom->parentname == NULL)
				{
					geom->parentname = new char[strlen(buf)+1];
					strcpy(geom->parentname,buf);
				}
			} 
			else if (!strcmp(g_keyword,tok_node_tm))
			{
				assert(geom);
				result |= ase_tmobject(stream,m,geom);
			}
			else if (!strcmp(g_keyword,tok_mesh))
			{
				assert(geom);
				result |= ase_mesh(stream, geom);
			}
			else if (!strcmp(g_keyword,tok_mesh_animation))
			{
				assert(geom);
				result |= ase_mesh_anim(stream,geom);
			}
			else if (!strcmp(g_keyword,tok_patchmesh))
			{
				assert(geom);
				result |= ase_patchmesh(stream, geom);
			}
			else if (!strcmp(g_keyword,tok_tm_animation))
			{
				assert(geom);
				result |= ase_tm_animation(stream,m,geom);
			}
			else if (!strcmp(g_keyword,tok_mesh_vertex_weightlist))
			{
				assert(geom);
				result |= ase_vertex_weight(stream,m,geom);
			}
			else if (!strcmp(g_keyword,tok_material_ref))
			{
				assert(geom);
				sscanf(g_leftover,"%d",&geom->matidx );

				if (geom->strip) geom->strip->matidx = geom->matidx;
			}

			//----ase optimal strip ---------by 2001-12-5
			else if (!strcmp(g_keyword,tok_strip))
			{
				result|= ase_stripobject(stream,geom);
			}
			//读入碰撞球体的信息 by sssa2000 090724
			else if (strcmp(g_keyword,tok_bound_sphere_info)==0)
			{
				//H3D::g_Log.OutPutInfo(H3D::RENDER_INFO,"g_keyword=%s",g_keyword);
				assert(geom);
				//while (ase_gettoken(stream, g_token))
				{
					//H3D::g_Log.OutPutInfo(H3D::RENDER_INFO,"	g_token=%s",g_token);
					geom->bound_type=BOUND_SPHERE;
					//对于包围球来说,前3个是球心,第四个是半径.
					sscanf(g_token, "%s %f %f %f %f",g_keyword, &geom->bound_data[0], &geom->bound_data[1], &geom->bound_data[2],&geom->bound_data[3]);
				}
				//result=1;
			}
			//读入碰撞胶囊体的信息 by sssa2000 090724
			else if (strcmp(g_keyword,tok_bound_capsule_info)==0)
			{
				assert(geom);

				//while (ase_gettoken(stream, g_token))
				{
					geom->bound_type=BOUND_CAPSULE;
					//对于胶囊来说,前三个是位置,第四个是高度,第五个是半径.
					sscanf(g_token, "%s %f %f %f %f %f",g_keyword, &geom->bound_data[0], &geom->bound_data[1], &geom->bound_data[2],&geom->bound_data[3],&geom->bound_data[4]);
				}
				//result=1;
			}
			//读入碰撞BOX体的信息 by sssa2000 090724
			else if (strcmp(g_keyword,tok_bound_box_info)==0)
			{
				assert(geom);
				geom->bound_type=BOUND_BOX;
				//对于Box来说,前三个是位置,第四个是长度,第五个是宽 第六个是高度.
				sscanf(g_token, "%s %f %f %f %f %f %f",g_keyword, &geom->bound_data[0], &geom->bound_data[1], &geom->bound_data[2],&geom->bound_data[3],&geom->bound_data[4],&geom->bound_data[5]);
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				assert(geom);
				m->geom.push_back(geom);

				if (parent == NULL)
					m->root_geom.push_back(geom);

				break;
			}
			else if (!strcmp(g_leftover,tok_begin))
			{
				ase_ignore(stream);
			}
		}
		return result;
	}
	int ase_cameraobject(const char * stream, model* m)
	{
		int result = 0;
		char buf[255];
		camobj* cam=NULL;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_node_name))
			{
				const char * pStart = strchr(g_token, '\"');
				const char * pEnd = strrchr(g_token, '\"');
				memcpy(buf,pStart,pEnd - pStart + 1);
				buf[pEnd - pStart + 1] = '\0';


				cam = new camobj;
				cam->name = new char[strlen(buf) + 1];
				strcpy(cam->name,buf);
			}
			else if (!strcmp(g_keyword,tok_node_tm))
			{
				float tm[16];
				float pos[3],rot[4],scl[3];
				assert(cam);
				result |= ase_gettm(stream,m,tm,pos,rot,scl,buf);

				if (strstr(buf,".Target"))
				{
				}
				else
				{
					cam->axis[0] = rot[0];
					cam->axis[1] = rot[1];
					cam->axis[2] = rot[2];
					cam->axisangle = rot[3];
					cam->pos[0] =tm[12];
					cam->pos[1] = tm[13];
					cam->pos[2] = tm[14];
				}
			}
			else if (!strcmp(g_keyword,tok_cam_settings))
			{
				assert(cam);
				result |= ase_camsettings(stream,cam);
			}
			else if (!strcmp(g_keyword, tok_cam_animation))
			{
				assert(cam);
				result |= ase_camanimation(stream,cam);
			}
			else if (!strcmp(g_keyword,tok_tm_animation))
			{
				assert(cam);
				geomobj * geom = new geomobj;
				result |= ase_tm_animation(stream,m,geom);
				cam->anim = geom->anim;
				geom->anim = 0;
				delete geom;
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				assert(cam);
				m->cam.push_back(cam);

				break;
			}
		}
		return result;
	}
	int ase_camsettings(const char* stream,camobj* cam)
	{
		int result = 0;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_cam_fov))
			{
				sscanf (g_token,"%s %f",g_keyword,&cam->fov);
			}
			else if (!strcmp(g_keyword,tok_cam_near))
			{
				sscanf (g_token,"%s %f",g_keyword,&cam->near_plane);
			}
			else if (!strcmp(g_keyword,tok_cam_far))
			{
				sscanf (g_token,"%s %f",g_keyword,&cam->far_plane);
			}
			else if (!strcmp(g_keyword,tok_cam_tdist))
			{
				sscanf (g_token,"%s %f",g_keyword,&cam->tdist);
			}
			else if (!strcmp(g_keyword,tok_close))
				break;
		}
		return result ;
	}

	int ase_camanimation(const char* stream,camobj* cam)
	{
		int result = 0;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_cam_settings))
			{
				result |= ase_camani_setting(stream,cam);
			}
			else if (!strcmp(g_keyword,tok_close))
				break;
		}
		return result;
	}

	int ase_camani_setting(const char* stream,camobj* cam)
	{
		int result = 0;
		t_fovkey key;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_cam_fov))
			{
				sscanf (g_token,"%s %f",g_keyword,&key.fov);
			}
			else if (!strcmp(g_keyword,tok_timevalue))
			{
				sscanf (g_token,"%s %d",g_keyword,&key.frmtime);
			}
			else if (!strcmp(g_keyword,tok_cam_near))
			{
				sscanf (g_token,"%s %f",g_keyword,&key.near_plane);
			}
			else if (!strcmp(g_keyword,tok_cam_far))
			{
				sscanf (g_token,"%s %f",g_keyword,&key.far_plane);
			}
			else if (!strcmp(g_keyword,tok_cam_tdist))
			{
				sscanf (g_token,"%s %f",g_keyword,&key.tdist);
			}
			else if (!strcmp(g_keyword,tok_close))
				break;
		}
		cam->cameraAni.push_back(key);
		return result ;
	}

	int ase_lightobject(const char * stream, model* m)
	{
		char buf[256];
		int result = 0;


		lightobj* light= NULL;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_node_name))
			{
				const char * pStart = strchr(g_token, '\"');
				const char * pEnd = strrchr(g_token, '\"');
				memcpy(buf,pStart,pEnd - pStart + 1);
				buf[pEnd - pStart + 1] = '\0';


				light = new lightobj;
				light->name = new char[strlen(buf) + 1];
				strcpy(light->name,buf);
			}
			else if (!strcmp(g_keyword,tok_light_type))
			{
				if (!strcmp(g_leftover,"Omni"))
					light->type = LIGHT_OMNI;
				else if (!strcmp(g_leftover,"Target"))
					light->type = LIGHT_TARGET;
				else light->type = LIGHT_OMNI;
			}
			else if (!strcmp(g_keyword,tok_node_tm))
			{
				float tm[16],pos[3],rot[4],scl[3];
				assert(light);
				result |= ase_gettm(stream,m,tm,pos,rot,scl,buf);
				if (strstr(buf,".Target"))
				{
					light->target[0] = tm[12];
					light->target[1] = tm[13];
					light->target[2] = tm[14];
				}
				else
				{
					light->pos[0] =tm[12];
					light->pos[1] = tm[13];
					light->pos[2] = tm[14];
					memcpy (light->tm,tm,16*sizeof(float));

				}
			}
			else if (!strcmp(g_keyword,tok_light_spotshape))
			{
				if (!strcmp(g_leftover,"Circle"))
				{
					light->spot_shape = 1;
				}
				else
				{
					light->spot_shape = 0;
				}
			}
			else if (!strcmp(g_keyword,tok_light_settings))
			{
				assert(light);
				result |= ase_lightsettings(stream,light);
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				assert(light);
				m->light.push_back(light);

				break;
			}
		}
		return result;
	}

	int ase_lightsettings(const char *stream, lightobj* light)
	{
		int result = 0;

		char buf[255];
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_light_color))
			{
				sscanf (g_token,"%s %f %f %f",g_keyword,&light->r,&light->g,&light->b);
			}
			else if (!strcmp(g_keyword,tok_light_intens))
			{
				sscanf (g_token,"%s %f",g_keyword,&light->intensity);
			}
			else if (!strcmp(g_keyword,tok_light_attnend))
			{
				sscanf (g_token,"%s %f",g_keyword,&light->attnend);
			}
			else if (!strcmp(g_keyword,tok_light_attnstart))
			{
				sscanf (g_token,"%s %f",g_keyword,&light->attnstart);
			}
			else if (!strcmp(g_keyword,tok_light_mapbias))
			{
				sscanf (g_token,"%s %f",g_keyword,&light->mapbias);
			}
			else if (!strcmp(g_keyword,tok_light_hotspot))
			{
				light->spot_hotspot = atof(g_leftover);
			}
			else if (!strcmp(g_keyword,tok_light_falloff))
			{
				light->spot_falloff = atof(g_leftover);
			}
			else if (!strcmp(g_keyword,tok_light_aspect))
			{
				light->spot_aspect = atof(g_leftover);
			}
			else if (!strcmp(g_keyword,tok_light_projmap))
			{
				const char * pStart = strchr(g_token, '\"');
				const char * pEnd = strrchr(g_token, '\"');

				if (pEnd-pStart+1>254)
				{
					memcpy(buf,pStart,254);
					buf[254]='\0';
				}

				else
				{
					memcpy(buf,pStart,pEnd - pStart + 1);

					buf[pEnd - pStart + 1] = '\0';
				}

				strcpy(light->spot_projmap,buf);
				light->hasProjmap=1;

			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}

	//pos[3];rot[4];scl[3];
	int ase_gettm(const char * stream, model * m,float* matrix,float* pos,float* rot,float *scl,char* nodename)
	{
		int result = 0;

		// We transpose while loading the row major to get a column major
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_node_name))
			{
				const char * pStart = strchr(g_token, '\"');
				const char * pEnd = strrchr(g_token, '\"');
				memcpy(nodename,pStart,pEnd - pStart + 1);
				nodename[pEnd - pStart + 1] = '\0';
			}
			else if (!strcmp(g_keyword,tok_tm_row0))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &matrix[0], &matrix[1], &matrix[2]);

			}
			else if (!strcmp(g_keyword,tok_tm_row1))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &matrix[4], &matrix[5], &matrix[6]);

			}
			else if (!strcmp(g_keyword,tok_tm_row2))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &matrix[8], &matrix[9], &matrix[10]);

			}
			else if (!strcmp(g_keyword,tok_tm_row3))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &matrix[12], &matrix[13], &matrix[14]);

				matrix[12] *= g_scale_hack;
				matrix[13] *= g_scale_hack;
				matrix[14] *= g_scale_hack;
			}
			else if (!strcmp(g_keyword,tok_tm_scale))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &scl[0], &scl[1], &scl[2]);
			}
			else if (!strcmp(g_keyword,tok_tm_pos))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &pos[0], &pos[1], &pos[2]);
			}
			else if (!strcmp(g_keyword,tok_tm_rotaxis))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &rot[0], &rot[1], &rot[2]);
			}
			else if (!strcmp(g_keyword,tok_tm_rotangle))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &rot[3]);
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				matrix[3] = 0.0f;
				matrix[7] = 0.0f;
				matrix[11] = 0.0f;
				matrix[15] = 1.0f;

				break;
			}
		}


		return result;
	}


	//----------------- ase optimal strip ---------------------
	int ase_stripobject(const char* stream, geomobj* geom)
	{
		int result = 0;
		geom->strip = new stripobj;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_strip_tvert))
			{
				sscanf(g_token,"%s %d",g_keyword,&geom->strip->hasTvert);
			}
			if (!strcmp(g_keyword,tok_strip_tvert2))
			{
				sscanf(g_token,"%s %d",g_keyword,&geom->strip->hasTvert2);
			}
			else if (!strcmp(g_keyword,tok_strip_normal))
			{
				sscanf(g_token,"%s %d",g_keyword,&geom->strip->hasNormal);
			}
			else if (!strcmp(g_keyword,tok_strip_tangent))
			{
				sscanf(g_token,"%s %d",g_keyword,&geom->strip->hasTangent);
			}
			else if (!strcmp(g_keyword,tok_strip_tangent_sign))
			{
				sscanf(g_token,"%s %d",g_keyword,&geom->strip->hasPMSign);
			}
			else if (!strcmp(g_keyword,tok_strip_vertcolor))
			{
				sscanf(g_token,"%s %d",g_keyword,&geom->strip->hasColor);
			}

			else if (!strcmp(g_keyword,tok_strip_numsubmtls))
			{
			}
			else if (!strcmp(g_keyword,tok_strip_submtls))
			{
				result |= ase_submatstrip(stream,geom->strip);
			}
			// add by lance 
			//else if (!strcmp(g_keyword, tok_lod)) {
			//	result |= ase_lodobject(stream, geom);
			//}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}

		return result;
	}

	int ase_stripVerts(const char* stream, stripobj* strip,t_strip_mat* stripmat)
	{
		int result = 0;

		int flag = 0;
		char buf[256],buf1[256];

		stripmat->v = new float [stripmat->numVert*3];

		if (strip->hasTvert)
		{//1
			stripmat->t = new float [stripmat->numVert*2];
			flag|=1<<0;

			//如果有2套UV，则必须有1套UV
			if(strip->hasTvert2)
			{
				stripmat->t2 = new float [stripmat->numVert*2];
			}
		}
		if (strip->hasColor)
		{//2
			// added alpha channel by lance
			stripmat->c = new float [stripmat->numVert*4];
			flag|=1<<1;
		}
		if (strip->hasNormal)
		{//4
			stripmat->n = new float [stripmat->numVert*3];
			flag|=1<<2;
		}
		if (strip->hasTangent)
		{
			// 切线
			stripmat->Tn = new float [stripmat->numVert*4];

			/// 初始化Sign
			for( unsigned int i = 0; i < stripmat->numVert; i++)
			{
				stripmat->Tn[i*4+3] = 1;  /// 默认无镜像
			}

			flag|=1<<3;
		}

		int num=0;
		int wnum = 0;
		int n;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_strip_vert3ds))
			{
				switch(flag)
				{
				case 0:
					sscanf(g_token,
						"%s %d: %f %f %f",
						g_keyword,
						&n,
						&stripmat->v[num*3],
						&stripmat->v[num*3+1],
						&stripmat->v[num*3+2]);
					break;
				case 1:
					if(strip->hasTvert2)
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f UV2: %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->t2[num*2],
							&stripmat->t2[num*2+1]);
					}
					else
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1]);
					}
					break;
				case 2:
					sscanf(g_token,
						"%s %d: %f %f %f VertColor: %f %f %f %f",
						g_keyword,
						&n,
						&stripmat->v[num*3],
						&stripmat->v[num*3+1],
						&stripmat->v[num*3+2],
						&stripmat->c[num*4],
						&stripmat->c[num*4+1],
						&stripmat->c[num*4+2],
						&stripmat->c[num*4+3]);

					break;
				case 3:
					if(strip->hasTvert2)
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f UV2: %f %f VertColor: %f %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->t2[num*2],
							&stripmat->t2[num*2+1],
							&stripmat->c[num*4],
							&stripmat->c[num*4+1],
							&stripmat->c[num*4+2],
							&stripmat->c[num*4+3]);
					}
					else
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f VertColor: %f %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->c[num*4],
							&stripmat->c[num*4+1],
							&stripmat->c[num*4+2],
							&stripmat->c[num*4+3]);
					}
					break;
				case 4:
					sscanf(g_token,
						"%s %d: %f %f %f Normal: %f %f %f",
						g_keyword,
						&n,
						&stripmat->v[num*3],
						&stripmat->v[num*3+1],
						&stripmat->v[num*3+2],
						&stripmat->n[num*3],
						&stripmat->n[num*3+1],
						&stripmat->n[num*3+2]);

					break;
				case 5:
					if(strip->hasTvert2)
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f UV2: %f %f Normal: %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->t2[num*2],
							&stripmat->t2[num*2+1],
							&stripmat->n[num*3],
							&stripmat->n[num*3+1],
							&stripmat->n[num*3+2]);
					}
					else
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f Normal: %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->n[num*3],
							&stripmat->n[num*3+1],
							&stripmat->n[num*3+2]);
					}
					break;
				case 6:
					sscanf(g_token,
						"%s %d: %f %f %f VertColor: %f %f %f %f Normal: %f %f %f",
						g_keyword,
						&n,
						&stripmat->v[num*3],
						&stripmat->v[num*3+1],
						&stripmat->v[num*3+2],
						&stripmat->c[num*4],
						&stripmat->c[num*4+1],
						&stripmat->c[num*4+2],
						&stripmat->c[num*4+3],
						&stripmat->n[num*3],
						&stripmat->n[num*3+1],
						&stripmat->n[num*3+2]	);

					break;
				case 7:
					if(strip->hasTvert2)
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f UV2: %f %f VertColor: %f %f %f %f Normal: %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->t2[num*2],
							&stripmat->t2[num*2+1],
							&stripmat->c[num*4],
							&stripmat->c[num*4+1],
							&stripmat->c[num*4+2],
							&stripmat->c[num*4+3],
							&stripmat->n[num*3],
							&stripmat->n[num*3+1],
							&stripmat->n[num*3+2]);
					}
					else
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f VertColor: %f %f %f %f Normal: %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->c[num*4],
							&stripmat->c[num*4+1],
							&stripmat->c[num*4+2],
							&stripmat->c[num*4+3],
							&stripmat->n[num*3],
							&stripmat->n[num*3+1],
							&stripmat->n[num*3+2]);
					}
					break;


				case 8:
					sscanf(g_token,
						"%s %d: %f %f %f Tangent: %f %f %f",
						g_keyword,
						&n,
						&stripmat->v[num*3],
						&stripmat->v[num*3+1],
						&stripmat->v[num*3+2],
						&stripmat->Tn[num*4],
						&stripmat->Tn[num*4+1],
						&stripmat->Tn[num*4+2],
						&stripmat->Tn[num*4+3]);
					break;
				case 9:
					if(strip->hasTvert2)
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f UV2: %f %f Tangent: %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->t2[num*2],
							&stripmat->t2[num*2+1],
							&stripmat->Tn[num*4],
							&stripmat->Tn[num*4+1],
							&stripmat->Tn[num*4+2],
							&stripmat->Tn[num*4+3]);
					}
					else
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f Tangent: %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->Tn[num*4],
							&stripmat->Tn[num*4+1],
							&stripmat->Tn[num*4+2],
							&stripmat->Tn[num*4+3]);
					}
					break;
				case 10:
					sscanf(g_token,
						"%s %d: %f %f %f VertColor: %f %f %f %f Tangent: %f %f %f %f",
						g_keyword,
						&n,
						&stripmat->v[num*3],
						&stripmat->v[num*3+1],
						&stripmat->v[num*3+2],
						&stripmat->c[num*4],
						&stripmat->c[num*4+1],
						&stripmat->c[num*4+2],
						&stripmat->c[num*4+3],
						&stripmat->Tn[num*4],
						&stripmat->Tn[num*4+1],
						&stripmat->Tn[num*4+2],
						&stripmat->Tn[num*4+3]);

					break;
				case 11:
					if(strip->hasTvert2)
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f UV2: %f %f VertColor: %f %f %f %f Tangent: %f %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->t2[num*2],
							&stripmat->t2[num*2+1],
							&stripmat->c[num*4],
							&stripmat->c[num*4+1],
							&stripmat->c[num*4+2],
							&stripmat->c[num*4+3],
							&stripmat->Tn[num*4],
							&stripmat->Tn[num*4+1],
							&stripmat->Tn[num*4+2],
							&stripmat->Tn[num*4+3]);
					}
					else
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f VertColor: %f %f %f %f Tangent: %f %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->c[num*4],
							&stripmat->c[num*4+1],
							&stripmat->c[num*4+2],
							&stripmat->c[num*4+3],
							&stripmat->Tn[num*4],
							&stripmat->Tn[num*4+1],
							&stripmat->Tn[num*4+2],
							&stripmat->Tn[num*4+3]);
					}
					break;
				case 12:
					sscanf(g_token,
						"%s %d: %f %f %f Normal: %f %f %f Tangent: %f %f %f %f",
						g_keyword,
						&n,
						&stripmat->v[num*3],
						&stripmat->v[num*3+1],
						&stripmat->v[num*3+2],
						&stripmat->n[num*3],
						&stripmat->n[num*3+1],
						&stripmat->n[num*3+2],
						&stripmat->Tn[num*4],
						&stripmat->Tn[num*4+1],
						&stripmat->Tn[num*4+2],
						&stripmat->Tn[num*4+3]);

					break;
				case 13:
					if(strip->hasTvert2)
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f UV2: %f %f Normal: %f %f %f Tangent: %f %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->t2[num*2],
							&stripmat->t2[num*2+1],
							&stripmat->n[num*3],
							&stripmat->n[num*3+1],
							&stripmat->n[num*3+2],
							&stripmat->Tn[num*4],
							&stripmat->Tn[num*4+1],
							&stripmat->Tn[num*4+2],
							&stripmat->Tn[num*4+3]);
					}
					else
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f Normal: %f %f %f Tangent: %f %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->n[num*3],
							&stripmat->n[num*3+1],
							&stripmat->n[num*3+2],
							&stripmat->Tn[num*4],
							&stripmat->Tn[num*4+1],
							&stripmat->Tn[num*4+2],
							&stripmat->Tn[num*4+3]);
					}
					break;
				case 14:
					sscanf(g_token,
						"%s %d: %f %f %f VertColor: %f %f %f %f Normal: %f %f %f Tangent: %f %f %f %f",
						g_keyword,
						&n,
						&stripmat->v[num*3],
						&stripmat->v[num*3+1],
						&stripmat->v[num*3+2],
						&stripmat->c[num*4],
						&stripmat->c[num*4+1],
						&stripmat->c[num*4+2],
						&stripmat->c[num*4+3],
						&stripmat->n[num*3],
						&stripmat->n[num*3+1],
						&stripmat->n[num*3+2],
						&stripmat->Tn[num*4],
						&stripmat->Tn[num*4+1],
						&stripmat->Tn[num*4+2],
						&stripmat->Tn[num*4+3]);

					break;
				case 15:
					if(strip->hasTvert2)
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f UV2: %f %f VertColor: %f %f %f %f Normal: %f %f %f Tangent: %f %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->t2[num*2],
							&stripmat->t2[num*2+1],
							&stripmat->c[num*4],
							&stripmat->c[num*4+1],
							&stripmat->c[num*4+2],
							&stripmat->c[num*4+3],
							&stripmat->n[num*3],
							&stripmat->n[num*3+1],
							&stripmat->n[num*3+2],
							&stripmat->Tn[num*4],
							&stripmat->Tn[num*4+1],
							&stripmat->Tn[num*4+2],
							&stripmat->Tn[num*4+3]);
					}
					else
					{
						sscanf(g_token,
							"%s %d: %f %f %f UV: %f %f VertColor: %f %f %f %f Normal: %f %f %f Tangent: %f %f %f %f",
							g_keyword,
							&n,
							&stripmat->v[num*3],
							&stripmat->v[num*3+1],
							&stripmat->v[num*3+2],
							&stripmat->t[num*2],
							&stripmat->t[num*2+1],
							&stripmat->c[num*4],
							&stripmat->c[num*4+1],
							&stripmat->c[num*4+2],
							&stripmat->c[num*4+3],
							&stripmat->n[num*3],
							&stripmat->n[num*3+1],
							&stripmat->n[num*3+2],
							&stripmat->Tn[num*4],
							&stripmat->Tn[num*4+1],
							&stripmat->Tn[num*4+2],
							&stripmat->Tn[num*4+3]);
					}
					break;
				default:
					result = 1;
					break;

				}//switch
				//assert (n==num);
				++num;
			}
			//just for physique exporting
			else if (!strcmp(g_keyword,tok_strip_vertweight))
			{
				if (!stripmat->wv)
					stripmat->wv = new t_weight[stripmat->numVert];

				ase_gettoken(stream, g_token);
				sscanf(g_token,"Influence_Bone: %d",&stripmat->wv[wnum].numbone);

				stripmat->wv[wnum].bonenames = new char*[stripmat->wv[wnum].numbone];
				stripmat->wv[wnum].weights = new float[stripmat->wv[wnum].numbone];

				for (int i=0;i<stripmat->wv[wnum].numbone;++i)
				{
					ase_gettoken(stream, buf);
					sscanf(buf,"Bname: %s",buf1);
					const char * pStart = strchr(buf, '\"');
					const char * pEnd = strrchr(buf, '\"');
					memcpy(g_token,pStart+1,pEnd - pStart -1);
					g_token[pEnd - pStart -1] = '\0';
					stripmat->wv[wnum].bonenames[i] = new char[strlen(g_token)+1];
					strcpy (stripmat->wv[wnum].bonenames[i],g_token);

					ase_gettoken(stream,g_token);
					sscanf(g_token,"Weight: %f",&stripmat->wv[wnum].weights[i]);

				}

				++wnum;
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}

		return result;
	}

	//static int strip_submat=0;
	int ase_submatstrip(const char* stream,stripobj* strip)
	{
		int result = 0;

		int numvert=0;
		int nStripMat = 0;

		t_strip_mat* stripmat=NULL;


		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_strip_submat))
			{
				sscanf(g_token,"%s %d",g_keyword, &nStripMat);
			}
			else if (!strcmp(g_keyword,tok_strip_numverts))
			{
				sscanf(g_token,"%s %d",g_keyword,&numvert);
				if (numvert>0)
				{
					stripmat = new t_strip_mat;
					stripmat->numVert = numvert;
					strip->strip_mat.push_back(stripmat);
				}
			}
			else if (!strcmp(g_keyword,tok_strip_submatverts))
			{//get 3ds verts
				if (numvert>0 && stripmat)
					result|=ase_stripVerts(stream,strip,stripmat);
				else
				{
					for (int i=0;i<4;++i)
						ase_gettoken(stream, g_token);
				}

			}
			else if (!strcmp(g_keyword,tok_strip_submat_strips))
			{
				result |= ase_getstrips(stream,stripmat,nStripMat);
			}
			// add by lance 2004-2-24, for lod importXml(generated by 3dsmax)
			else if(!strcmp(g_keyword, tok_lod_3dsmax)){
				result |= ase_getlods(stream, stripmat,nStripMat);
			}
			// add by lance 2004-3-2, for lod importXml(generated by program)
			else if (!strcmp(g_keyword, tok_lod)) {
				result |= ase_lodobject(stream, stripmat);

				// t_strip_mat ends here
				numvert = 0;
				stripmat = NULL;
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}///while 

		return result;
	}
	static int s_nSmoothGroup=0;
	int ase_getstrips(const char* stream, t_strip_mat* stripmat,int nStripMat)
	{
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_strip))
			{
				ase_readstrips(stream, stripmat,nStripMat);
			}
			else if (!strcmp(g_keyword,tok_strip_smooth))
			{
				s_nSmoothGroup = atoi(g_leftover);
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}

		return result;

	}

	int ase_readstrips(const char* stream, t_strip_mat* stripmat,int nStripMat)
	{
		int result = 0;


		stripmat->submat = nStripMat;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_strip_token))
			{
				t_strip* str = new t_strip;
				str->smooth = s_nSmoothGroup;
				str->submat = nStripMat;
				ase_param_strip(g_token,str);

				if (!str->elems.size()) 
				{
					delete str;
					continue;
				}

				stripmat->strips.push_back(str);


			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}

		return result;
	}
	void ase_param_strip(const char* token,t_strip* str)
	{
		//strip string: *Strips %d: %d %d %d %d .."
		int elem;
		int pos = strcspn(token,":")+2;

		for (int i=pos;i<strlen(token);++i)
		{
			sscanf (token+i,"%s",g_keyword);

			elem = atoi(g_keyword);

			str->elems.push_back(elem);

			i+=strlen(g_keyword);
		}
	}

	int ase_readlodstrips(const char* stream, strip_array* striparray,int nStripMat)
	{
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_strip_token))
			{
				t_strip* str = new t_strip;
				str->smooth = s_nSmoothGroup;
				str->submat = nStripMat;
				ase_param_strip(g_token,str);

				if (!str->elems.size()) 
				{
					delete str;
					continue;
				}

				striparray->push_back(str);


			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}

		return result;
	}

	int ase_getlods(const char* stream, t_strip_mat* stripmat,int nStripMat)
	{
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_strip_submat_strips_lod))
			{
				strip_array* striparray = new strip_array;
				stripmat->lods.push_back(striparray);
				ase_getlodstrips(stream, striparray,nStripMat);
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}

		return result;
	}

	int ase_getlodstrips(const char* stream, strip_array* striparray,int nStripMat)
	{
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_strip))
			{
				ase_readlodstrips(stream, striparray,nStripMat);
			}
			else if (!strcmp(g_keyword,tok_strip_smooth))
			{
				s_nSmoothGroup = atoi(g_leftover);
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}


		return result;
	}

	int ase_tmobject(const char * stream, model * m, geomobj * geom)
	{
		int result = 0;

		// We transpose while loading the row major to get a column major
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			/*
			if (!strcmp(g_keyword,tok_node_name))
			{
			const char * pStart = strchr(token, '\"');
			} 
			else */
			if (!strcmp(g_keyword,tok_tm_row0))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &geom->tm[0], &geom->tm[1], &geom->tm[2]);
			}
			else if (!strcmp(g_keyword,tok_tm_row1))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &geom->tm[4], &geom->tm[5], &geom->tm[6]);
			}
			else if (!strcmp(g_keyword,tok_tm_row2))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &geom->tm[8], &geom->tm[9], &geom->tm[10]);
			}
			else if (!strcmp(g_keyword,tok_tm_row3))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &geom->tm[12], &geom->tm[13], &geom->tm[14]);
				geom->tm[12] *= g_scale_hack;
				geom->tm[13] *= g_scale_hack;
				geom->tm[14] *= g_scale_hack;
			}
			else if (!strcmp(g_keyword,tok_tm_scale))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &geom->scale[0], &geom->scale[1], &geom->scale[2]);
			}
			else if (!strcmp(g_keyword,tok_tm_pos))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &geom->pos[0], &geom->pos[1], &geom->pos[2]);
			}
			else if (!strcmp(g_keyword,tok_tm_rotaxis))
			{
				sscanf(g_token, "%s %f %f %f",g_keyword, &geom->rotaxis[0], &geom->rotaxis[1], &geom->rotaxis[2]);
			}
			else if (!strcmp(g_keyword,tok_tm_rotangle))
			{
				sscanf(g_token, "%s %f",g_keyword, &geom->rotangle);
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				geom->tm[3] = 0.0f;
				geom->tm[7] = 0.0f;
				geom->tm[11] = 0.0f;
				geom->tm[15] = 1.0f;

				// store the inverse...Assume this is a rigid body tm
				glh::matrix4f mat(geom->tm);
				glh::matrix4f invmat = mat.inverse();
				memcpy(geom->bone_offset_tm,invmat.get_value(),sizeof(float) * 16);
				break;
			}
		}
		return result;
	}

	int ase_patchmesh(const char * stream, geomobj * geom)
	{
		int result = 0;

		geom->type = geomobj::patched;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			// control vertices
			if (!strcmp(g_keyword,tok_patchmesh_vertex_list))
			{
				result |= ase_patchmesh_vertex_list(stream,geom);
			} 
			else if (!strcmp(g_keyword,tok_patchmesh_vector_list))
			{
				result |= ase_patchmesh_vector_list(stream,geom);
			} 
			else if (!strcmp(g_keyword,tok_patchmesh_edge_list))
			{
				result |= ase_patchmesh_edge_list(stream,geom);
			} 
			else if (!strcmp(g_keyword,tok_patchmesh_patch_list))
			{
				result |= ase_patchmesh_patch_list(stream,geom);
			} 
			else if (!strcmp(g_keyword,tok_patchmesh_tvchannel_list))
			{
				result |= ase_patchmesh_tvchannel_list(stream,geom);
			} 
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}

	int ase_patchmesh_vertex_list(const char * stream, geomobj * geom)
	{
		int result = 0;
		int idx, idx_times_3;
		float x, y, z;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_patchmesh_vertex))
			{
				sscanf(g_token, "%s %d %f %f %f",g_keyword, &idx, &x, &y, &z);
				idx_times_3 = idx * 3;
				geom->verts[idx_times_3] = x * g_scale_hack;
				geom->verts[idx_times_3 + 1] = y * g_scale_hack;
				geom->verts[idx_times_3 + 2] = z * g_scale_hack;
			}
			else if (!strcmp(g_keyword,tok_patchmesh_numverts))
			{
				sscanf(g_token, "%s %d",g_keyword, &geom->numverts);
				geom->verts = new float[geom->numverts * 3];
			} 
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}

	int ase_patchmesh_vector_list(const char * stream, geomobj * geom)
	{
		int result = 0;
		int idx, idx_times_3;
		float x, y, z;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_patchmesh_vector))
			{
				sscanf(g_token, "%s %d %f %f %f",g_keyword, &idx, &x, &y, &z);
				idx_times_3 = idx * 3;
				geom->vecs[idx_times_3] = x * g_scale_hack;
				geom->vecs[idx_times_3 + 1] = y * g_scale_hack;
				geom->vecs[idx_times_3 + 2] = z * g_scale_hack;
			}
			else if (!strcmp(g_keyword,tok_patchmesh_numvecs))
			{
				sscanf(g_token, "%s %d",g_keyword, &geom->numvecs);
				geom->vecs = new float[geom->numvecs * 3];
			} 
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}

	int ase_patchmesh_edge_list(const char * stream, geomobj * geom)
	{
		int result = 0;
		int idx, idx_times_4;
		int i0, i1, i2, i3;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_patchmesh_edge))
			{
				sscanf(g_token, "%s %d %d %d %d %d",g_keyword, &idx, &i0, &i1, &i2, &i3);
				idx_times_4 = idx * 4;
				geom->edges[idx_times_4] = i0;
				geom->edges[idx_times_4 + 1] = i1;
				geom->edges[idx_times_4 + 2] = i2;
				geom->edges[idx_times_4 + 3] = i3;
			}
			else if (!strcmp(g_keyword,tok_patchmesh_numedges))
			{
				sscanf(g_token, "%s %d",g_keyword, &geom->numedges);
				geom->edges = new int[geom->numedges * 4];
			} 
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}

	int ase_patchmesh_patch_list(const char * stream, geomobj * geom)
	{
		int result = 0;
		int idx;
		int e0, e1, e2, e3;
		int i0, i1, i2, i3;
		int smg,numedges;
		int mtlid;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_patchmesh_patch))
			{
				sscanf(g_token, "%s %d %d %d %d %d %d %d %d %d %d, %d, %d",g_keyword, &idx, &numedges, &e0, &e1, &e2, &e3, &i0, &i1, &i2, &i3, &smg, &mtlid);
				geom->patches[idx].numedges = numedges;
				geom->patches[idx].edge[0] = e0;
				geom->patches[idx].edge[1] = e1;
				geom->patches[idx].edge[2] = e2;
				geom->patches[idx].edge[3] = e3;
				geom->patches[idx].interior[0] = i0;
				geom->patches[idx].interior[1] = i1;
				geom->patches[idx].interior[2] = i2;
				geom->patches[idx].interior[3] = i3;
				geom->patches[idx].smg = smg;
				geom->patches[idx].mtlid = mtlid;
			}
			else if (!strcmp(g_keyword,tok_patchmesh_numpatches))
			{
				sscanf(g_token, "%s %d",g_keyword, &geom->numpatches);
				geom->patches = new patch[geom->numpatches];
			} 
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}

	int ase_patchmesh_tvchannel_list(const char * stream, geomobj * geom)
	{
		int result = 0;
		int tvchannel_idx= 0;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_patchmesh_tvchannel))
			{
				result |= ase_patchmesh_tvchannel(stream, geom, tvchannel_idx);
				tvchannel_idx++;
			}
			else if (!strcmp(g_keyword,tok_patchmesh_numtvchannels))
			{
				sscanf(g_token, "%s %d",g_keyword, &geom->numtvchannels);

				geom->numtvverts = new int[geom->numtvchannels];
				geom->tvverts = new float*[geom->numtvchannels];
				geom->tvpatches = new int*[geom->numtvchannels];
			} 
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}

	int ase_patchmesh_tvchannel(const char * stream, geomobj * geom, int tvchannel_idx)
	{
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_patchmesh_tvvert_list))
			{
				result |= ase_patchmesh_tvvert_list(stream, geom, tvchannel_idx);
			}
			else if (!strcmp(g_keyword,tok_patchmesh_tvpatch_list))
			{
				result |= ase_patchmesh_tvpatch_list(stream, geom, tvchannel_idx);
			} 
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}

	int ase_patchmesh_tvvert_list(const char * stream, geomobj * geom, int tvchannel_idx)
	{
		int result = 0;
		int idx, idx_times_3;
		float u,v,w;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_patchmesh_tvvert))
			{
				sscanf(g_token, "%s %d %f %f %f",g_keyword, &idx, &u, &v, &w);
				idx_times_3 = idx * 3;
				geom->tvverts[tvchannel_idx][idx_times_3] = u;
				geom->tvverts[tvchannel_idx][idx_times_3 + 1] = -v;
				geom->tvverts[tvchannel_idx][idx_times_3 + 2] = w;
			}
			else if (!strcmp(g_keyword,tok_patchmesh_numtvverts))
			{
				sscanf(g_token, "%s %d",g_keyword, &geom->numtvverts[tvchannel_idx]);
				geom->tvverts[tvchannel_idx] = new float[geom->numtvverts[tvchannel_idx] * 3];
				if (geom->numtvverts[tvchannel_idx])
					geom->tvpatches[tvchannel_idx] = new int[geom->numtvverts[tvchannel_idx] * 4];
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}

	int ase_patchmesh_tvpatch_list(const char * stream, geomobj * geom, int tvchannel_idx)
	{
		int result = 0;
		int idx, idx_times_4;
		int i0, i1, i2, i3;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if (!strcmp(g_keyword,tok_patchmesh_tvpatch))
			{
				sscanf(g_token, "%s %d %d %d %d %d",g_keyword, &idx, &i0, &i1, &i2, &i3);
				idx_times_4 = idx * 4;
				geom->tvpatches[tvchannel_idx][idx_times_4] = i0;
				geom->tvpatches[tvchannel_idx][idx_times_4 + 1] = i1;
				geom->tvpatches[tvchannel_idx][idx_times_4 + 2] = i2;
				geom->tvpatches[tvchannel_idx][idx_times_4 + 3] = i3;
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}
	//add by puzzy, load animation meshes
	int	ase_mesh_anim(const char * stream, geomobj * geom)
	{
		int result=0;
		geomobj* animstrip=NULL;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);


			if (!strcmp(g_keyword,tok_mesh))
			{
				assert(geom);
				geomobj* animmesh = new geomobj;
				animstrip = animmesh;
				animmesh->matidx = geom->matidx;
				result |= ase_mesh(stream, animmesh);
				if (animmesh->numv)
					geom->anim_mesh.push_back(animmesh);

			}
			//----ase optimal strip ---------by 2001-12-5
			else if (!strcmp(g_keyword,tok_strip))
			{
				result|= ase_stripobject(stream,animstrip);
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}

	int ase_mesh(const char * stream, geomobj * geom)
	{
		int result = 0;

		/*static int first = 0;
		if ( first == 0)
		{
			MessageBox(NULL, "test", "test", MB_OK);
			first = 1;
		}*/

		geom->type = geomobj::polygonal;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			// number of vertices
			if (!strcmp(g_keyword,tok_mesh_numvertices))
			{
				sscanf(g_leftover, "%d",&geom->numv);
				if (geom->numv)
					geom->v = new float[geom->numv * 3];
			}
			//time value
			else if (!strcmp(g_keyword,tok_timevalue))
			{
				sscanf(g_leftover,"%d",&geom->timevalue);
			}
			// number of faces indices
			else if (!strcmp(g_keyword,tok_mesh_numfaces))
			{
				sscanf(g_leftover, "%d",&geom->numf);
				if (geom->numf)
				{
					geom->f = new unsigned int[geom->numf * 3];
					geom->fsubmat = new unsigned int[geom->numf];
					geom->smg = new int[geom->numf];
				}
			}
			// number of texture coords
			else if (!strcmp(g_keyword,tok_mesh_numtvertices))
			{
				sscanf(g_leftover, "%d",&geom->numt );
				if (geom->numt)
					geom->t = new float[geom->numt * 3];
			}
			else if (!strcmp(g_keyword,tok_mesh_numtvertlist))
			{
				result |= ase_texturevertexlist(stream,geom);
			}
			// -------------vertex color //add by puzzy 2001-11-6----------------
			else if (!strcmp(g_keyword,tok_mesh_numcvertex))
			{
				sscanf(g_leftover, "%d",&geom->numc );
				if (geom->numc)
					geom->c = new float[geom->numc * 3];
			}
			else if (!strcmp(g_keyword,tok_mesh_numcvfaces))
			{
				sscanf(g_leftover, "%d",&geom->numcf );
				if (geom->numcf)
					geom->cf = new unsigned int[geom->numcf * 3];
			}
			else if (!strcmp(g_keyword,tok_mesh_cfacelist))
			{
				result |= ase_colorfacelist(stream,geom);
			}
			else if (!strcmp(g_keyword,tok_mesh_cvertlist))
			{
				result |= ase_colorvertexlist(stream,geom);
			}
			//---------------------vertex color-------------------

			else if (!strcmp(g_keyword,tok_mesh_numtvfaces))
			{
				sscanf(g_leftover, "%d",&geom->numtf );
				if (geom->numtf)
					geom->tf = new unsigned int[geom->numtf * 3];
			}
			else if (!strcmp(g_keyword,tok_mesh_vertexlist))
			{
				result |= ase_vertexlist(stream, geom);
			}
			else if (!strcmp(g_keyword,tok_mesh_facelist))
			{
				result |= ase_facelist(stream, geom);
			}
			else if (!strcmp(g_keyword,tok_mesh_normals))
			{
				if (geom->numf)
				{
					geom->fn = new float[geom->numf * 3];
					geom->fvn = new unsigned int[geom->numf * 3];
				}

				result |= ase_normals(stream,geom);
			}
			else if (!strcmp(g_keyword,tok_mesh_tangents))
			{
				if (geom->numf)
				{
					geom->fvTn = new unsigned int[geom->numf * 3];
				}

				/*result |= */ase_tangents(stream,geom);
			}
			else if (!strcmp(g_keyword,tok_mesh_facemaplist))
			{
				if (geom->numf)
					geom->fmapv = new unsigned int[geom->numf * 3 * 3];
				result |= ase_facemaplist(stream,geom);

			}
			else if (!strcmp(g_keyword,tok_mesh_tfacelist))
			{
				result |= ase_texturefacelist(stream,geom, 0);
			}
			else if (!strcmp(g_keyword,tok_mesh_mappingchannel))
			{
				//by sssa2000 090925
				int mapIdx=0;
				sscanf(g_leftover, "%d",&mapIdx);
				result |= ase_mapchannel(stream,geom,mapIdx-1); //max是从1开始算的
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}
	int ase_texturevertexlist_null(const char * stream, geomobj * geom)
	{
		int result = 0;
		float x, y, z;
		int   index;


		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword, tok_mesh_tvert))
			{
				sscanf(g_token, "%s %d %f %f %f",g_keyword, &index, &x, &y, &z);
			} 
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}   
		return result;
	}
	int ase_texturefacelist_null(const char * stream, geomobj * geom)
	{
		int result = 0;
		int x, y, z;
		int   index;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_mesh_tface))
			{
				sscanf(g_token, "%s %d %d %d %d",g_keyword, &index, &x, &y, &z);
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}   
		return result;
	}
	int ase_mapchannel(const char* stream,geomobj* geom,int mapIdx)
	{
		int result = 0;
		while (ase_gettoken(stream,g_token))
		{
			sscanf(g_token,"%s %s",g_keyword,g_leftover);
			if (!strcmp(g_keyword,tok_mesh_numtvertices))
			{
				int numt2;
				sscanf(g_leftover, "%d",&numt2);
				if (geom->numt!=numt2)
				{
				}

				if (numt2)
				{
					//暂时使用patchs' info
					/*
					numtvverts = new int [1];
					numtvverts[0] = numt2;
					tvverts = new (float*)[1] ;
					tvverts[0] = new float[numt2*3]
					*/
					ase_texturevertexlist_idx(stream,geom,mapIdx,numt2);
				}
			}
			else if (!strcmp(g_keyword,tok_mesh_numtvertlist))
			{
				result |= ase_texturevertexlist_null(stream,geom);
			}
			else if ( !strcmp(g_keyword,tok_mesh_numtvfaces) )
			{
				if (mapIdx == 1)
				{
					sscanf(g_leftover, "%d",&geom->numtf1);
					if (geom->numtf1)
						geom->tf1 = new unsigned int[geom->numtf1 * 3];
				}
			}
			else if ( !strcmp(g_keyword,tok_mesh_tfacelist) )
			{
				result |= ase_texturefacelist(stream,geom, mapIdx);
				//result |= ase_texturefacelist_null(stream,geom);
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}
	int j(const char * stream, geomobj * geom)
	{
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_mesh_face))
			{
				int    i1, i2, i3;
				int    index;
				int    AB, BC, CA;
				int    smg;
				int    submatidx;

				sscanf(g_token, "%s %d: A: %d  B: %d  C: %d AB: %d BC: %d CA: %d *MESH_SMOOTHING %d",
					g_keyword, &index, &i1, &i2, &i3, &AB, &BC, &CA, &smg);

				assert(index < geom->numf);

				char * pBuf = strrchr(g_token,'*');
				sscanf(pBuf,"*MESH_MTLID %d", &submatidx);

				assert(submatidx >= 0);
				geom->fsubmat[index] = submatidx;
				//assert(smg >= 0);
				geom->smg[index] = (smg == -1) ? 0 : smg;
				index *= 3;
				geom->f[index] = i1;
				geom->f[index + 1] = i2;
				geom->f[index + 2] = i3;
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}   
		return result;
	}

	int ase_vertexlist(const char * stream, geomobj * geom)
	{
		char g_keyword[32];
		char g_leftover[256];
		int result = 0;
		float x, y, z;
		int   index;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_mesh_vertex))
			{
				sscanf(g_token, "%s %d %f %f %f",g_keyword, &index, &x, &y, &z);
				assert(geom->numv > index);

				index *= 3;
				geom->v[index ] = x * g_scale_hack;
				geom->v[index + 1] = y * g_scale_hack;
				geom->v[index + 2] = z * g_scale_hack;
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}   
		return result;
	}
	int ase_facelist(const char * stream, geomobj * geom)
	{
		int result = 0; 
		while (ase_gettoken(stream, g_token))
		{ 
			sscanf(g_token, "%s %s",g_keyword, g_leftover); 
			if (!strcmp(g_keyword,tok_mesh_face))
			{ 
				int i1, i2, i3;
				unsigned int index;
				int AB, BC, CA; 
				int smg; 
				int submatidx; 
				sscanf(g_token, "%s %d: A: %d B: %d C: %d AB: %d BC: %d CA: %d *MESH_SMOOTHING %d", g_keyword, &index, &i1, &i2, &i3, &AB, &BC, &CA, &smg); 
				assert(index < geom->numf); 
				char * pBuf = strrchr(g_token,'*'); 
				sscanf(pBuf,"*MESH_MTLID %d", &submatidx); 
				assert(submatidx >= 0); 
				geom->fsubmat[index] = submatidx; //assert(smg >= 0);
				geom->smg[index] = (smg == -1) ? 0 : smg; index *= 3;

				geom->f[index] = i1; geom->f[index + 1] = i2;

				geom->f[index + 2] = i3;

		 }
			else if (!strcmp(g_keyword,tok_close)) 
			{
				break;
			} 
		} 
		return result; 
	} 

	int ase_normals(const char * stream, geomobj * geom)
	{
		float x, y, z;
		int   index;
		int result = 0;

		int faceindex;
		int inc;
		std::vector<float>   normals;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_mesh_facenormal))
			{
				sscanf(g_token, "%s %d %f %f %f",g_keyword, &index, &x, &y, &z);

				assert(index < geom->numf);
				index *= 3;
				faceindex = index;
				geom->fn[index ] = x;
				geom->fn[index + 1] = y;
				geom->fn[index + 2] = z;
				inc = 0;
			}
			if (!strcmp(g_keyword,tok_mesh_vertexnormal))
			{
				sscanf(g_token, "%s %d %f %f %f",g_keyword, &index, &x, &y, &z);

				normals.push_back(x);
				normals.push_back(y);
				normals.push_back(z);
				geom->fvn[faceindex + inc] = normals.size() / 3 - 1;
				++inc;
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				geom->numn = normals.size() / 3;
				geom->n = new float[geom->numn * 3];

				for (int i= 0; i < normals.size(); ++i)
					geom->n[i] = normals[i];
				break;
			}
		}
		return result;
	}
	/// 切线增加手相性
	int ase_tangents(const char * stream, geomobj * geom)
	{
		float x, y, z, tmp;
		int   index;
		int result = 0;

		int faceindex;
		int inc = 0;
		std::vector<float>   tangents;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_mesh_facenormal))
			{
				sscanf(g_token, "%s %d %f %f %f",g_keyword, &index, &x, &y, &z);

				assert(index < geom->numf);
				index *= 3;
				faceindex = index;
				/*geom->fn[index ] = x;/// faceNormal不用在读取了
				geom->fn[index + 1] = y;
				geom->fn[index + 2] = z;*/
				inc = 0;
			}
			if (!strcmp(g_keyword,tok_mesh_vertextangent))
			{
				sscanf(g_token, "%s %d %f %f %f %f",g_keyword, &index, &x, &y, &z, &tmp);

				tangents.push_back(x);
				tangents.push_back(y);
				tangents.push_back(z);
				if (tmp == -1)
				{
					tangents.push_back(tmp);
				}
				else
				{
					tangents.push_back(1);
				}
				geom->fvTn[faceindex + inc] = tangents.size() / 4 - 1;
				++inc;
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				geom->numTn = tangents.size() / 4;
				geom->Tn = new float[geom->numTn * 4];

				for (int i= 0; i < tangents.size(); ++i)
					geom->Tn[i] = tangents[i];
				break;
			}
		}
		return result;
	}

	int ase_facemaplist(const char * stream, geomobj * geom)
	{
		char g_keyword[32];
		char g_leftover[256];
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword, tok_mesh_facemap))
			{
				int index;
				sscanf(g_token, "%s %d",g_keyword, &index);
				result |= ase_facemapvert(stream, geom, index);
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}   
		return result;
	}

	int ase_facemapvert(const char * stream, geomobj * geom, int index)
	{
		int result = 0;
		int inc = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword, tok_mesh_facemapvert))
			{
				unsigned int a, b, c;
				sscanf(g_token, "%s %d %d %d",g_keyword, &a, &b, &c);
				geom->fmapv[geom->f[index * 3 + inc]] = a;
				geom->fmapv[geom->f[index * 3 + inc] + 1] = b;
				geom->fmapv[geom->f[index * 3 + inc] + 2] = c;
				inc++; 
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}   
		return result;
	}

	int ase_texturevertexlist(const char * stream, geomobj * geom)
	{
		int result = 0;
		float x, y, z;
		int   index;


		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword, tok_mesh_tvert))
			{
				sscanf(g_token, "%s %d %f %f %f",g_keyword, &index, &x, &y, &z);
				index *= 3;
				geom->t[index ] = x;
				geom->t[index + 1] = y; // convert from v to t
				geom->t[index + 2] = z;
			} 
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}   
		return result;
	}

	int ase_texturevertexlist_idx(const char * stream, geomobj * geom,int tc_idx,unsigned int number_of_elem) //by sssa2000 090924
	{
		if (tc_idx>3)
		{
			//::MessageBoxA(0,"Ase Do Not Support more than 3 texcoord",0,0);
			return -1;
		}

		int result = 0;
		float x, y, z;
		int   index;
		geom->CreateTc(tc_idx,number_of_elem);

		float* tc_select[]={geom->t,geom->t1,geom->t2,geom->t3};
		float* current_tc=tc_select[tc_idx];
		

		if (!current_tc)
		{
			::MessageBoxA(0,"texcoord is null",0,0);
			return -1;
		}

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword, tok_mesh_tvert))
			{
				sscanf(g_token, "%s %d %f %f %f",g_keyword, &index, &x, &y, &z);
				index *= 3;
				current_tc[index ] = x;
				current_tc[index + 1] = y; // convert from v to t
				current_tc[index + 2] = z;
			} 
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}   
		return result;
	}


	int ase_texturefacelist(const char * stream, geomobj * geom, int mapIdx)
	{
		int result = 0;
		int x, y, z;
		int   index;

		unsigned int* tf = NULL;
		int numtf = 0;
		if (mapIdx == 0)
		{
			numtf = geom->numtf;
			tf = geom->tf;
		}
		else if (mapIdx == 1)
		{
			numtf = geom->numtf1;
			tf = geom->tf1;
		}

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_mesh_tface))
			{
				sscanf(g_token, "%s %d %d %d %d",g_keyword, &index, &x, &y, &z);
				assert(index < numtf);
				index *= 3;
				tf[index] = x;
				tf[index + 1] = y;
				tf[index + 2] = z;
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}   
		return result;
	}

	//get vertices color
	int ase_colorfacelist(const char* stream, geomobj *geom)//add by puzzy,2001-11-6
	{
		int result = 0;
		int x, y, z;
		int   index;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_mesh_cface))
			{
				sscanf(g_token, "%s %d %d %d %d",g_keyword, &index, &x, &y, &z);
				assert(index < geom->numcf);
				index *= 3;
				geom->cf[index] = x;
				geom->cf[index + 1] = y;
				geom->cf[index + 2] = z;
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}   
		return result;
	}

	int ase_colorvertexlist(const char* stream, geomobj *geom)//add by puzzy,2001-11-6
	{
		int result = 0;
		float x, y, z;
		int   index;


		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword, tok_mesh_vertcol))
			{
				sscanf(g_token, "%s %d %f %f %f",g_keyword, &index, &x, &y, &z);
				index *= 3;
				geom->c[index ] = x;
				geom->c[index + 1] = y; // convert from v to t
				geom->c[index + 2] = z;
			}
			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}   
		return result;
	}



	int ase_tm_animation(const char * stream,model* localmodel,geomobj * geom)
	{
		int result = 0;

		geom->anim = new animdata;
		geom->anim->firstfrm = localmodel->firstfrm;
		geom->anim->lastfrm = localmodel->lastfrm;
		geom->anim->framespeed = localmodel->framespeed;
		geom->anim->tickperfrm = localmodel->tickperfrm;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_control_pos_track))
				result |= ase_postrack(stream,geom->anim);
			else if (!strcmp(g_keyword,tok_control_rot_track))
				result |= ase_rottrack(stream,geom->anim);
			else if (!strcmp(g_keyword,tok_control_scale_track))
				//result |= ase_skip(stream);
				result |= ase_scaletrack(stream, geom->anim);
			//add by puzzy
			else if (!strcmp(g_keyword,tok_control_pos_linear)) //linear pos
				result |= ase_poskey(stream,geom->anim);
			else if (!strcmp(g_keyword,tok_control_rot_linear)) //linear rot
				result |= ase_rotkey(stream,geom->anim);
			else if (!strcmp(g_keyword,tok_control_scale_linear)) //linear scale
				result |= ase_sclkey(stream,geom->anim);
			else if (!strcmp(g_keyword,tok_control_pos_tcb)) //tcb pos
				result |= ase_tcbposkey(stream,geom->anim);
			else if (!strcmp(g_keyword,tok_control_rot_tcb)) //tcb rot
				result |= ase_tcbrotkey(stream,geom->anim);
			else if (!strcmp(g_keyword,tok_control_scale_tcb)) //tcb scale
				result |= ase_tcbsclkey(stream,geom->anim);
			else if (!strcmp(g_keyword,tok_control_pos_bezier)) //tcb pos
				result |= ase_bezierposkey(stream,geom->anim);
			else if (!strcmp(g_keyword,tok_control_scale_bezier)) //tcb scale
				result |= ase_beziersclkey(stream,geom->anim);

			else if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}
	int ase_poskey(const char * stream,animdata * anim)
	{
		int result = 0;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_control_pos_key))
			{
				float x,y,z;
				int t;

				sscanf(g_token,"%s %d %f %f %f %f",g_keyword,&t, &x, &y, &z);
				t_poskey key;
				key.frmtime = t;
				key.pos[0] = x;
				key.pos[1] = y;
				key.pos[2] = z;
				key.type = ANIM_LINEAR;
				anim->pos.push_back(key);
			}
			else if (!strcmp(g_keyword,tok_close))
				break;
		}
		return result;
	}
	void ase_axis2quat(float* arr)
	{
		//arr[0],arr[1],arr[2] is an axis
		//arr[3] is rad
		//change arr to quaternion
		float fHalfAngle = 0.5f*arr[3];
		float fSin = sinf(fHalfAngle);
		arr[3] = cosf(fHalfAngle);
		arr[0] = fSin*arr[0];
		arr[1] = fSin*arr[1];
		arr[2] = fSin*arr[2];
	}
	int ase_rotkey(const char * stream,animdata * anim)
	{
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_control_rot_key))
			{
				float x,y,z,w;
				int t;

				sscanf(g_token,"%s %d %f %f %f %f",g_keyword,&t, &x, &y, &z,&w);
				t_rotkey key;
				key.frmtime = t;
				key.quat[0] = x;
				key.quat[1] = y;
				key.quat[2] = z;
				key.quat[3] = w;
				//	ase_axis2quat(key.quat);
				key.type = ANIM_LINEAR;
				anim->rot.push_back(key);
			}
			else if (!strcmp(g_keyword,tok_close))
				break;
		}

		return result;

	}
	int ase_sclkey(const char * stream,animdata * anim)
	{
		int result = 0;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_control_scale_key))
			{
				float scl[7];
				int t;

				sscanf(g_token,"%s %d %f %f %f %f %f %f %f",g_keyword,&t, 
					&scl[0], 
					&scl[1], 
					&scl[2], 
					&scl[3], 
					&scl[4], 
					&scl[5], 
					&scl[6]
				);
				t_sclkey key;
				key.frmtime = t;
				key.scl[0] = scl[0];
				key.scl[1] = scl[1];
				key.scl[2] = scl[2];
				key.scl[3] = scl[3];
				key.scl[4] = scl[4];
				key.scl[5] = scl[5];
				key.scl[6] = scl[6];
				key.type = ANIM_LINEAR;

				anim->scl.push_back(key);

			}
			else if (!strcmp(g_keyword,tok_close))
				break;
		}
		return result;

	}
	int ase_tcbposkey(const char * stream,animdata * anim)
	{
		int result = 0;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_control_pos_tcbkey))
			{
				float x,y,z;
				int t;
				t_poskey key;
				sscanf(g_token,"%s %d %f %f %f %f",g_keyword,&t, &x, &y, &z,
					&key.tcbparam[0],
					&key.tcbparam[1],
					&key.tcbparam[2],
					&key.tcbparam[3],
					&key.tcbparam[4]
				);

				key.frmtime = t;
				key.pos[0] = x;
				key.pos[1] = y;
				key.pos[2] = z;
				key.type = ANIM_TCB;
				anim->pos.push_back(key);
			}
			else if (!strcmp(g_keyword,tok_close))
				break;
		}
		return result;

	}
	int ase_tcbrotkey(const char * stream,animdata * anim)
	{
		int result = 0;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_control_rot_tcbkey))
			{
				float x,y,z,w;
				int t;
				t_rotkey key;

				sscanf(g_token,"%s %d %f %f %f %f",g_keyword,&t, &x, &y, &z,&w,
					&key.tcbparam[0],&key.tcbparam[1],&key.tcbparam[2],&key.tcbparam[3],&key.tcbparam[4]);

				key.frmtime = t;
				key.quat[0] = x;
				key.quat[1] = y;
				key.quat[2] = z;
				key.quat[3] = w;
				//ase_axis2quat(key.quat);
				key.type = ANIM_TCB;
				anim->rot.push_back(key);
			}
			else if (!strcmp(g_keyword,tok_close))
				break;
		}
		return result;

	}
	int ase_tcbsclkey(const char * stream,animdata * anim)
	{
		int result = 0;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_close))
			{

				break;
			}
		}

		return result;

	}
	int ase_bezierposkey(const char * stream,animdata * anim)
	{
		int result = 0;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_close))
			{

				break;
			}
		}

		return result;
	}
	int ase_beziersclkey(const char * stream,animdata * anim)
	{
		int result = 0;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_close))
			{

				break;
			}
		}
		return result;

	}


	int ase_scaletrack(const char * stream,animdata * anim)
	{
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_control_scale_sample))
			{
				float x,y,z;
				int t;

				sscanf(g_token,"%s %d %f %f %f",g_keyword,&t, &x, &y, &z);
				t_sclkey key;
				key.type = ANIM_SAMPLE;
				key.scl[0] = x;
				key.scl[1] = y;
				key.scl[2] = z;
				key.frmtime = t;
				anim->scl.push_back(key);
			}
			else if (!strcmp(g_keyword,tok_close))
			{

				break;
			}
		}
		return result;
	}


	int ase_postrack(const char * stream,animdata * anim)
	{
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_control_pos_sample))
			{
				float x,y,z;
				int t;

				sscanf(g_token,"%s %d %f %f %f %f",g_keyword,&t, &x, &y, &z);
				t_poskey key;
				key.type = ANIM_SAMPLE;
				key.pos[0] = x;
				key.pos[1] = y;
				key.pos[2] = z;
				key.frmtime = t;
				anim->pos.push_back(key);
			}
			else if (!strcmp(g_keyword,tok_close))
			{

				break;
			}
		}
		return result;
	}

	int ase_rottrack(const char * stream,animdata * anim)
	{
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_control_rot_sample))
			{
				float x,y,z,w;
				int t;

				sscanf(g_token,"%s %d %f %f %f %f",g_keyword, &t, &x, &y, &z, &w);
				glh::quaternionf q(x,y,z,w);
				q.conjugate();

				t_rotkey key;
				key.type = ANIM_SAMPLE;
				key.quat[0] = x;
				key.quat[1] = y;
				key.quat[2] = z;
				key.quat[3] = w;
				key.frmtime = t;
				anim->rot.push_back(key);
			}
			else if (!strcmp(g_keyword,tok_close))
			{

				break;
			}
		}
		return result;
	}

	int ase_vertex_weight(const char * stream, model * m, geomobj * geom)
	{
		char buf[256];
		int result = 0;
		float weight;
		float off_x, off_y, off_z;
		int v_idx;
		int i_times_4;

		std::vector<float>          bvstack;
		std::vector<float*>         bvmatstack;
		std::vector<geomobj*>       bvgeomstack;
		std::vector<unsigned int>   vbvstack;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword, tok_mesh_vertex_weight))
			{
				const char * pStart = strchr(g_token, '\"');
				sscanf(g_token, "%s %d",g_keyword,&v_idx);
				const char * pEnd = strrchr(g_token, '\"');
				strncpy(buf,pStart,pEnd - pStart + 1);
				buf[pEnd - pStart + 1] = '\0';

				sscanf(pEnd + 1,"%f %f %f %f", &off_x, &off_y, &off_z, &weight);

				geomobj * geomref = get_geomobj(m, buf);
				// the reference must exist!
				if (geomref == NULL)
				{
					geomref = new geomobj;
					geomref->name = new char[strlen(buf) + 1];
					strcpy(geomref->name,buf);
					m->geom.push_back(geomref);
				}

				// push back all data
				bvgeomstack.push_back(geomref);
				bvmatstack.push_back(geomref->tm);

				vbvstack.push_back(v_idx);

				bvstack.push_back(off_x * g_scale_hack);
				bvstack.push_back(off_y * g_scale_hack);
				bvstack.push_back(off_z * g_scale_hack);
				bvstack.push_back(weight);
			}
			else if (!strcmp(g_keyword,"}"))
			{
				geom->numbv = bvstack.size() / 4;
				if (geom->numbv)
				{
					geom->bv = new float[4 * geom->numbv];
					geom->bmatref = new float*[geom->numbv];
					geom->bgeomref = new geomobj*[geom->numbv];
					geom->vbv = new unsigned int[geom->numbv];

					for (int i = 0; i < geom->numbv; ++i)
					{
						i_times_4 = i * 4;
						geom->bv[i_times_4] = bvstack[i_times_4];
						geom->bv[i_times_4 + 1] = bvstack[i_times_4 + 1];
						geom->bv[i_times_4 + 2] = bvstack[i_times_4 + 2];
						geom->bv[i_times_4 + 3] = bvstack[i_times_4 + 3];

						geom->vbv[i] = vbvstack[i];

						geom->bmatref[i] = bvmatstack[i];

						geom->bgeomref[i] = bvgeomstack[i];
					}
				}
				break;
			}
		}   
		return result;
	}

	int ase_passthru(const char * stream, model * m)
	{
		int result = 0;

		int back_stream_cnt = g_stream_cnt;
		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
			else
			{
				g_stream_cnt = back_stream_cnt;
				result |= ase_parse(stream, m);
			}
		}

		return result;
	}

	int ase_skip(const char * stream)
	{
		int result = 0;

		while (ase_gettoken(stream, g_token))
		{
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if (!strcmp(g_keyword,tok_close))
			{
				break;
			}
		}
		return result;
	}


	//////////////////////////////////////////////////////////////////////////
	// by lance 2003-8-12
	// add AGAIN at 2003-12-18

	int ase_readlod(const char* stream, lodobj* lo);
	int ase_readlod1(const char* stream, lodobj* lo);
	int ase_readstrip(const char* stream, lodobj* lo);
	int ase_readtriangles(const char* stream, lodobj* lo);

	int ase_lodobject(const char* stream, t_strip_mat* stripmat)
	{
		int result = 0;

		while(ase_gettoken(stream, g_token)){
			sscanf(g_token, "%s %s",g_keyword, g_leftover);
			if(!strcmp(g_keyword, tok_lod_numlevels)){
			}
			else if(!strcmp(g_keyword, tok_lod_depth)){
				lodobj* lo = new lodobj();
				sscanf(g_token, "%s %f", g_keyword, &lo->depth);
				result |= ase_readlod(stream, lo);
				stripmat->lodlist.push_back(lo);			
			}
			else if(!strcmp(g_keyword, tok_close)){
				break;
			}
		}

		return result;

	}

	int ase_readlod(const char* stream, lodobj* lo)
	{
		int result = 0;

		while(ase_gettoken(stream, g_token)){
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if(!strcmp(g_keyword, tok_lod_numfaces)){
				sscanf(g_token, "%s %d", g_keyword, &lo->num_faces);
			}
			else if(!strcmp(g_keyword, tok_lod1)){
				result |= ase_readlod1(stream, lo);
			}
			else if(!strcmp(g_keyword, tok_close)){
				break;
			}
		}

		return result;

	}

	int ase_readlod1(const char* stream, lodobj* lo)
	{
		int result = 0;

		while(ase_gettoken(stream, g_token)){
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if(!strcmp(g_keyword, tok_lod_striplength)){
				sscanf(g_token, "%s %d", g_keyword, &lo->striplength);
				lo->pStrip = new unsigned short[lo->striplength];
				result |= ase_readstrip(stream, lo);
			}

			if(!strcmp(g_keyword, tok_lod_trianglecount)){
				int trianglecount = 0;
				sscanf(g_token, "%s %d", g_keyword, &trianglecount);
				lo->indicescount = trianglecount * 3;
				lo->pIndices = new unsigned short[lo->indicescount];
			}
			else if(!strcmp(g_keyword, tok_lod_triangles)){
				result |= ase_readtriangles(stream, lo);
				break;
			}
		}

		return result;

	}

	int ase_readstrip(const char* stream, lodobj* lo)
	{
		int result = 0;
		int n = 0;
		unsigned short* p = lo->pStrip;
		std::stringstream ssin;
		std::string str;
		while(ase_gettoken(stream, g_token)){
			sscanf(g_token, "%s %s", g_keyword, g_leftover);

			if(strcmp(g_keyword, tok_lod_strip)){
				break;
			}

			ssin.clear();
			ssin.str(g_token);

			ssin>>str;

			while(ssin>>str){
				*p++ = static_cast<unsigned short>(atoi(str.c_str()));
				++n;
			}


		}
		assert(n==lo->striplength);
		return result;
	}

	int ase_readtriangles(const char* stream, lodobj* lo)
	{
		int result = 0;

		int n = 0;
		unsigned short* p = lo->pIndices;
		int v1,v2,v3;
		while(ase_gettoken(stream, g_token)){
			sscanf(g_token, "%s %s",g_keyword, g_leftover);

			if(!strcmp(g_keyword, tok_lod_triangle))
			{
				sscanf(g_token, "%s %s %d %d %d", g_keyword, g_leftover, &v1, &v2, &v3);
				*p++ = (unsigned short)v1;
				*p++ = (unsigned short)v2;
				*p++ = (unsigned short)v3;
				n+=3;
			}
			else if(!strcmp(g_keyword, tok_close)){
				break;
			}
		}

		assert(n==lo->indicescount);

		return result;

	}



	//////////////////////////////////////////////////////////////////////////

	lodobj::lodobj():num_faces(0),pStrip(0),pIndices(0),striplength(0),indicescount(0)
	{
	}

	lodobj::~lodobj()
	{
		//deleted by yanshuai
		/*if (striplength)*/
		if (pStrip != NULL)
		{
			delete []pStrip;
			pStrip = NULL;
		}

		//deleted by yanshuai
		/*if (indicescount)*/
		if (pIndices != NULL)
		{
			delete []pIndices;
			pIndices = NULL;
		}
	}

	t_strip_mat::~t_strip_mat()
	{
		strip_array* striparray;
		t_strip* str;
		for(int i = 0; i < lods.size(); ++i){
			striparray = lods[i];
			for(int j = 0; j < striparray->size(); ++j){
				str = (*striparray)[j];
				delete str;
			}
			delete striparray;
		}

		// add by lance
		for(int i = 0; i < lodlist.size(); ++i){
			delete lodlist[i];
		}
	}

}
#pragma  warning(pop)