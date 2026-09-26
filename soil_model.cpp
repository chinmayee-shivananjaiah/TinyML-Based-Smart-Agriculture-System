#include "soil_model.h"

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "model-parameters/model_metadata.h"

#include <cstring>

static float soil_features[4];

static int get_soil_data(size_t offset, size_t length, float *out_ptr)
{
    memcpy(out_ptr,
           &soil_features[offset],
           length * sizeof(float));

    return 0;
}

int soil_predict(float moisture,
                 float ph,
                 float rainfall,
                 float humidity)
{
    // Store input features in the same order
    // used during Edge Impulse model development.
    soil_features[0] = moisture;
    soil_features[1] = ph;
    soil_features[2] = rainfall;
    soil_features[3] = humidity;

    signal_t signal;
    signal.total_length = 4;
    signal.get_data = get_soil_data;

    ei_impulse_result_t result = {};

    // Run TinyML inference
    EI_IMPULSE_ERROR res =
        run_classifier(&signal, &result, false);

    if (res != EI_IMPULSE_OK)
    {
        return -1;
    }

    // Find the class with the highest probability
    int best_class = 0;
    float best_value = result.classification[0].value;

    for (int i = 1; i < EI_CLASSIFIER_LABEL_COUNT; i++)
    {
        if (result.classification[i].value > best_value)
        {
            best_value = result.classification[i].value;
            best_class = i;
        }
    }

    return best_class;
}