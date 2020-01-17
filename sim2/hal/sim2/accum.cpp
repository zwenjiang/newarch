#include <hal/Ports.h>
#include <hal/AnalogAccumulator.h>
#include <hal/Errors.h>

int32_t HAL_GetNumAccumulators()
{
    return 0 ;
}

HAL_Bool HAL_IsAccumulatorChannel(HAL_AnalogInputHandle analogPortHandle, int32_t* status)
{
    return false ;
}

void HAL_InitAccumulator(HAL_AnalogInputHandle analogPortHandle, int32_t* status)
{
    *status = HAL_SIM_NOT_SUPPORTED ;
}                         

void HAL_ResetAccumulator(HAL_AnalogInputHandle analogPortHandle, int32_t* status)
{  
    *status = HAL_SIM_NOT_SUPPORTED ;      
}

void HAL_SetAccumulatorCenter(HAL_AnalogInputHandle analogPortHandle, int32_t center, int32_t* status)
{
    *status = HAL_SIM_NOT_SUPPORTED ;    
}

void HAL_SetAccumulatorDeadband(HAL_AnalogInputHandle analogPortHandle,
                                int32_t deadband, int32_t* status)
{
    *status = HAL_SIM_NOT_SUPPORTED ;    
}

int64_t HAL_GetAccumulatorValue(HAL_AnalogInputHandle analogPortHandle,
                                int32_t* status)
{
    *status = HAL_SIM_NOT_SUPPORTED ;    
    return 0;
}

int64_t HAL_GetAccumulatorCount(HAL_AnalogInputHandle analogPortHandle,
                                int32_t* status)
{
    *status = HAL_SIM_NOT_SUPPORTED ;    
    return 0;
}

void HAL_GetAccumulatorOutput(HAL_AnalogInputHandle analogPortHandle,
                              int64_t* value, int64_t* count, int32_t* status)
{
    *status = HAL_SIM_NOT_SUPPORTED ;    
}
