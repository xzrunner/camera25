#include "c25_camera.h"

#include <sm_c_matrix.h>
#include <sm_c_vector.h>
#include <sm_const.h>

#include <stdlib.h>
#include <math.h>

struct var_cache {
	union sm_mat4 mvp_mat;

	float tan_rad;
	float cos_rad_inv;
};

struct c25_camera {
	struct sm_vec3 pos;
	float angle;	// in degree

	float aspect;

	union sm_mat4 mv_mat, proj_mat;

	struct var_cache c;
};

static void
_update_var_cache(struct c25_camera* cam) {
	sm_mat4_mul(&cam->c.mvp_mat, &cam->proj_mat, &cam->mv_mat);

	float rad = - cam->angle * SM_DEG_TO_RAD;
	cam->c.tan_rad = tanf(rad);
	cam->c.cos_rad_inv = 1 / cosf(rad);
}

static void
_cal_mat(struct c25_camera* cam) {
	union sm_mat4 rmat;
	sm_mat4_rotxmat(&rmat, cam->angle);

	union sm_mat4 tmat;
	sm_mat4_identity(&tmat);
	sm_mat4_trans(&tmat, cam->pos.x, cam->pos.y, cam->pos.z);

	sm_mat4_mul(&cam->mv_mat, &rmat, &tmat);

	sm_mat4_perspective(&cam->proj_mat, 45, cam->aspect, 0.01f, 9999);

	_update_var_cache(cam);
}

struct c25_camera*
c25_cam_create(const struct sm_vec3* pos, float angle, float aspect) {
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

void
c25_cam_set_pos(struct c25_camera* cam, const struct sm_vec3* pos) {
	cam->pos = *pos;
	_cal_mat(cam);
}

void
c25_cam_set_angle(struct c25_camera* cam, float angle) {
	cam->angle = angle;
	_cal_mat(cam);
}

const union sm_mat4*
c25_cam_get_modelview_mat(const struct c25_camera* cam) {
	return &cam->mv_mat;
}

const union sm_mat4*
c25_cam_get_project_mat(const struct c25_camera* cam) {
	return &cam->proj_mat;
}

const struct sm_vec3*
c25_cam_get_pos(const struct c25_camera* cam) {
	return &cam->pos;
}

float
c25_cam_get_angle(const struct c25_camera* cam) {
	return cam->angle;
}

struct sm_vec2*
c25_screen_to_world(const struct c25_camera* cam, struct sm_vec2* world,
					const struct sm_ivec2* screen, int sw, int sh) {
	float dy = 2.0f * (sh - screen->y) / sh - 1;
	if (cam->c.tan_rad * dy == 1) {
		world->x = world->y = 0;
		return world;
	}

	float z = cam->pos.z * cam->c.cos_rad_inv;
	float dz = cam->c.tan_rad * dy * z / (1 - cam->c.tan_rad * dy);
	z += dz;

	float aspect = (float)(sw) / sh;
	float dx = 	(2.0f * screen->x / sw - 1) * aspect;

	float offy = cam->c.tan_rad * cam->pos.z;
	world->x = -dx * z - cam->pos.x;
	world->y = -dy * z * cam->c.cos_rad_inv - cam->pos.y - offy;
	return world;
}

struct sm_ivec2*
c25_world_to_screen(const struct c25_camera* cam, struct sm_ivec2* screen,
					const struct sm_vec3* world, int sw, int sh) {
	struct sm_vec3 vec = *world;
	sm_vec3_mul(&vec, &cam->c.mvp_mat);
	screen->x = (int)((vec.x + 1) * 0.5f * sw);
	screen->y = (int)((vec.y + 1) * 0.5f * sh);
	return screen;
}