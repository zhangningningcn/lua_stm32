MatrixFunctionsOBJ := arm_mat_add_f32.o arm_mat_add_q15.o arm_mat_add_q31.o arm_mat_init_f32.o \
arm_mat_init_q15.o arm_mat_init_q31.o arm_mat_inverse_f32.o arm_mat_mult_f32.o \
arm_mat_mult_fast_q15.o arm_mat_mult_fast_q31.o arm_mat_mult_q15.o arm_mat_mult_q31.o \
arm_mat_scale_f32.o arm_mat_scale_q15.o arm_mat_scale_q31.o arm_mat_sub_f32.o \
arm_mat_sub_q15.o arm_mat_sub_q31.o arm_mat_trans_f32.o arm_mat_trans_q15.o \
arm_mat_trans_q31.o 

OBJ += $(MatrixFunctionsOBJ)
