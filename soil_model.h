#ifndef SOIL_MODEL_H
#define SOIL_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

int soil_predict(float moisture,
                 float ph,
                 float rainfall,
                 float humidity);

#ifdef __cplusplus
}
#endif

#endif