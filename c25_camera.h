#ifdef __cplusplus
extern "C"
{
#endif

#ifndef camera25_camera_h
#define camera25_camera_h

#include <sm.h>

struct c25_camera;

struct c25_camera* c25_cam_create(struct sm_vec3* pos, float angle, float aspect);
void c25_cam_release(struct c25_camera*);

void c25_cam_translate(struct c25_camera*, struct sm_vec3* offset);
void c25_cam_rotate(struct c25_camera*, float da);
void c25_cam_set_pos(struct c25_camera*, const struct sm_vec3* pos);
void c25_cam_set_angle(struct c25_camera*, float angle);

const union sm_mat4* c25_cam_get_modelview_mat(const struct c25_camera*);
const union sm_mat4* c25_cam_get_project_mat(const struct c25_camera*);

const struct sm_vec3* c25_cam_get_pos(const struct c25_camera*);
float c25_cam_get_angle(const struct c25_camera*);

struct sm_vec2*  c25_screen_to_world(const struct c25_camera*, struct sm_vec2* world, const struct sm_ivec2* screen, int sw, int sh);
struct sm_ivec2* c25_world_to_screen(const struct c25_camera*, struct sm_ivec2* screen, const struct sm_vec3* world, int sw, int sh);

#endif // camera25_camera_h

#ifdef __cplusplus
}
#endif