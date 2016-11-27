BasicMathFunctionsOBJ := arm_abs_f32.o arm_abs_q15.o arm_abs_q31.o arm_abs_q7.o \
arm_add_f32.o arm_add_q15.o arm_add_q31.o arm_add_q7.o \
arm_dot_prod_f32.o arm_dot_prod_q15.o arm_dot_prod_q31.o arm_dot_prod_q7.o \
arm_mult_f32.o arm_mult_q15.o arm_mult_q31.o arm_mult_q7.o \
arm_negate_f32.o arm_negate_q15.o arm_negate_q31.o arm_negate_q7.o \
arm_offset_f32.o arm_offset_q15.o arm_offset_q31.o arm_offset_q7.o \
arm_scale_f32.o arm_scale_q15.o arm_scale_q31.o arm_scale_q7.o \
arm_shift_q15.o arm_shift_q31.o arm_shift_q7.o arm_sub_f32.o \
arm_sub_q15.o arm_sub_q31.o arm_sub_q7.o 

OBJ += $(BasicMathFunctionsOBJ)
