#ifndef __POSTURE_DEF
#define __POSTURE_DEF

#ifdef __cplusplus
 extern "C" {
#endif
     
typedef struct
{
    float roll;
    float pitch;
    float yaw;
    
}POSTURE_INFO_T;    
     
        

void Posture_Start(void);
     
     
     
     
#ifdef __cplusplus
}
#endif
#endif
