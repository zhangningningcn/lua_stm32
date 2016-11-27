TransformFunctionsOBJ := arm_cfft_radix4_f32.o arm_cfft_radix4_init_f32.o arm_cfft_radix4_init_q15.o arm_cfft_radix4_init_q31.o \
arm_cfft_radix4_q15.o arm_cfft_radix4_q31.o arm_dct4_f32.o arm_dct4_init_f32.o \
arm_dct4_init_q15.o arm_dct4_init_q31.o arm_dct4_q15.o arm_dct4_q31.o \
arm_rfft_f32.o arm_rfft_init_f32.o arm_rfft_init_q15.o arm_rfft_init_q31.o \
arm_rfft_q15.o arm_rfft_q31.o 

OBJ += $(TransformFunctionsOBJ)
