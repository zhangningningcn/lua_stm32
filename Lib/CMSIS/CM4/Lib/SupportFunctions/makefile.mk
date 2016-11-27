SupportFunctionsOBJ := arm_copy_f32.o arm_copy_q15.o arm_copy_q31.o arm_copy_q7.o \
arm_fill_f32.o arm_fill_q15.o arm_fill_q31.o arm_fill_q7.o \
arm_float_to_q15.o arm_float_to_q31.o arm_float_to_q7.o arm_q15_to_float.o \
arm_q15_to_q31.o arm_q15_to_q7.o arm_q31_to_float.o arm_q31_to_q15.o \
arm_q31_to_q7.o arm_q7_to_float.o arm_q7_to_q15.o arm_q7_to_q31.o 

OBJ += $(SupportFunctionsOBJ)
