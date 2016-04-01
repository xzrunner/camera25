#include "c25_camera.h"

#include <stdlib.h>

struct c25_camera {
	struct sm_vec3 pos;
	float angle;	// in degree

	float aspect;

	union sm_mat4 mat;
};

static void
_cal_mat(struct c25_camera* cam) {
	union sm_mat4 rmat;
	sm_mat4_rotxmat(&rmat, cam->angle);

	union sm_mat4 tmat;
	sm_mat4_identity(&tmat);
	sm_mat4_trans(&tmat, cam->pos.x, cam->pos.y, cam->pos.z);

	union sm_mat4 mv_mat;
	sm_mat4_mul(&mv_mat, &rmat, &tmat);

	union sm_mat4 proj_mat;
	sm_mat4_perspective(&proj_mat, -cam->aspect, cam->aspect, -1, 1, 1, 9999);

	sm_mat4_mul(&cam->mat, &proj_mat, &mv_mat);
}

struct c25_camera* 
c25_cam_create(struct sm_vec3* pos, float angle, float aspect) {
	struct c25_camera* cam = malloc(sizeof(*cam));

	cam->pos = *pos;
	cam->angle = angle;
	cam->aspect = aspect;

	_cal_mat(cam);	

	return cam;
}

void 
c25_cam_release(struct c25_camera* cam) {
	free(cam);
}

void 
c25_cam_reset(struct c25_camera* cam, struct sm_vec3* pos, float angle) {
	cam->pos = *pos;
	cam->angle = angle;
	_cal_mat(cam);
}

void 
c25_cam_translate(struct c25_camera* cam, struct sm_vec3* offset) {
	cam->pos.x += offset->x;
	cam->pos.y += offset->y;
	cam->pos.z += offset->z;
	_cal_mat(cam);
}

void 
c25_cam_rotate(struct c25_camera* cam, float da) {
	cam->angle += da;
	_cal_mat(cam);
}

const struct sm_vec3* 
c25_cam_get_pos(struct c25_camera* cam) {
	return &cam->pos;
}

float 
c25_cam_get_angle(struct c25_camera* cam) {
	return cam->angle;
}

struct sm_vec2*  
c25_screen_to_world(struct c25_camera* cam, struct sm_vec2* world, 
					const struct sm_ivec2* screen, int sw, int sh) {
	float rad = - cam->angle * SM_DEG_TO_RAD;
	float tan_rad = tanf(rad);
	float dy = 2.0f * (sh - screen->y) / sh - 1;
	if (tan_rad * dy == 1) {
		world->x = world->y = 0;
		return world;
	}

	float cos_rad = cosf(rad);
	float z = cam->pos.z / cos_rad;
	float dz = tan_rad * dy * z / (1 - tan_rad * dy);
	z += dz;

	float aspect = (float)(sw) / sh;
	float dx = 	(2.0f * screen->x / sw - 1) * aspect;

	float offy = tan_rad * cam->pos.z;
	world->x = -dx * z - cam->pos.x;
	world->y = -dy * z / cos_rad - cam->pos.y - offy;
	return world;
}

struct sm_ivec2* 
c25_world_to_screen(struct c25_camera* cam, struct sm_ivec2* screen, 
					const struct sm_vec3* world, int sw, int sh) {
	struct sm_vec3 vec = *world;
	sm_vec3_mul(&vec, &cam->mat);
	screen->x = (vec.x + 1) * 0.5f * sw;
	screen->y = (vec.y + 1) * 0.5f * sh;
	return screen;
}