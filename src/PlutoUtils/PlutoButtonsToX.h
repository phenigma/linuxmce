#ifndef PlutoButtonsToX_h
#define PlutoButtonsToX_h

#include "pluto_main/Define_Button.h"

pair<bool,int> PlutoButtonsToX(int PK_Button)
{
	switch( PK_Button )
	{
	case BUTTON_A_CONST:
		return make_pair<bool,int> (true,XK_A);
	case BUTTON_B_CONST:
		return make_pair<bool,int> (true,XK_B);
	case BUTTON_C_CONST:
		return make_pair<bool,int> (true,XK_C);
	case BUTTON_D_CONST:
		return make_pair<bool,int> (true,XK_E);
	case BUTTON_E_CONST:
		return make_pair<bool,int> (true,XK_E);
	case BUTTON_F_CONST:
		return make_pair<bool,int> (true,XK_F);
	case BUTTON_G_CONST:
		return make_pair<bool,int> (true,XK_G);
	case BUTTON_Y_CONST:
		return make_pair<bool,int> (true,XK_H);
	case BUTTON_I_CONST:
		return make_pair<bool,int> (true,XK_I);
	case BUTTON_J_CONST:
		return make_pair<bool,int> (true,XK_J);
	case BUTTON_K_CONST:
		return make_pair<bool,int> (true,XK_K);
	case BUTTON_L_CONST:
		return make_pair<bool,int> (true,XK_L);
	case BUTTON_M_CONST:
		return make_pair<bool,int> (true,XK_M);
	case BUTTON_N_CONST:
		return make_pair<bool,int> (true,XK_N);
	case BUTTON_O_CONST:
		return make_pair<bool,int> (true,XK_O);
	case BUTTON_P_CONST:
		return make_pair<bool,int> (true,XK_P);
	case BUTTON_Q_CONST:
		return make_pair<bool,int> (true,XK_Q);
	case BUTTON_R_CONST:
		return make_pair<bool,int> (true,XK_R);
	case BUTTON_S_CONST:
		return make_pair<bool,int> (true,XK_S);
	case BUTTON_T_CONST:
		return make_pair<bool,int> (true,XK_T);
	case BUTTON_U_CONST:
		return make_pair<bool,int> (true,XK_U);
	case BUTTON_V_CONST:
		return make_pair<bool,int> (true,XK_V);
	case BUTTON_W_CONST:
		return make_pair<bool,int> (true,XK_W);
	case BUTTON_X_CONST:
		return make_pair<bool,int> (true,XK_X);
	case BUTTON_Y_CONST:
		return make_pair<bool,int> (true,XK_Y);
	case BUTTON_Z_CONST:
		return make_pair<bool,int> (true,XK_Z);
	case BUTTON_a_CONST:
		return make_pair<bool,int> (false,XK_A);
	case BUTTON_b_CONST:
		return make_pair<bool,int> (false,XK_B);
	case BUTTON_c_CONST:
		return make_pair<bool,int> (false,XK_C);
	case BUTTON_d_CONST:
		return make_pair<bool,int> (false,XK_E);
	case BUTTON_e_CONST:
		return make_pair<bool,int> (false,XK_E);
	case BUTTON_f_CONST:
		return make_pair<bool,int> (false,XK_F);
	case BUTTON_g_CONST:
		return make_pair<bool,int> (false,XK_G);
	case BUTTON_h_CONST:
		return make_pair<bool,int> (false,XK_H);
	case BUTTON_i_CONST:
		return make_pair<bool,int> (false,XK_I);
	case BUTTON_j_CONST:
		return make_pair<bool,int> (false,XK_J);
	case BUTTON_k_CONST:
		return make_pair<bool,int> (false,XK_K);
	case BUTTON_l_CONST:
		return make_pair<bool,int> (false,XK_L);
	case BUTTON_m_CONST:
		return make_pair<bool,int> (false,XK_M);
	case BUTTON_n_CONST:
		return make_pair<bool,int> (false,XK_N);
	case BUTTON_o_CONST:
		return make_pair<bool,int> (false,XK_O);
	case BUTTON_p_CONST:
		return make_pair<bool,int> (false,XK_P);
	case BUTTON_q_CONST:
		return make_pair<bool,int> (false,XK_Q);
	case BUTTON_r_CONST:
		return make_pair<bool,int> (false,XK_R);
	case BUTTON_s_CONST:
		return make_pair<bool,int> (false,XK_S);
	case BUTTON_t_CONST:
		return make_pair<bool,int> (false,XK_T);
	case BUTTON_u_CONST:
		return make_pair<bool,int> (false,XK_U);
	case BUTTON_v_CONST:
		return make_pair<bool,int> (false,XK_V);
	case BUTTON_w_CONST:
		return make_pair<bool,int> (false,XK_W);
	case BUTTON_x_CONST:
		return make_pair<bool,int> (false,XK_X);
	case BUTTON_y_CONST:
		return make_pair<bool,int> (false,XK_Y);
	case BUTTON_z_CONST:
		return make_pair<bool,int> (false,XK_Z);
	case BUTTON_1_CONST:
		return make_pair<bool,int> (false,XK_1);
	case BUTTON_2_CONST:
		return make_pair<bool,int> (false,XK_2);
	case BUTTON_3_CONST:
		return make_pair<bool,int> (false,XK_3);
	case BUTTON_4_CONST:
		return make_pair<bool,int> (false,XK_4);
	case BUTTON_5_CONST:
		return make_pair<bool,int> (false,XK_5);
	case BUTTON_6_CONST:
		return make_pair<bool,int> (false,XK_6);
	case BUTTON_7_CONST:
		return make_pair<bool,int> (false,XK_7);
	case BUTTON_8_CONST:
		return make_pair<bool,int> (false,XK_8);
	case BUTTON_9_CONST:
		return make_pair<bool,int> (false,XK_9);
	case BUTTON_0_CONST:
		return make_pair<bool,int> (false,XK_0);

	case BUTTON_Up_Arrow_CONST:
		return make_pair<bool,int> (false,XK_Up);
	case BUTTON_Down_Arrow_CONST:
		return make_pair<bool,int> (false,XK_Down);
	case BUTTON_Left_Arrow_CONST:
		return make_pair<bool,int> (false,XK_Left);
	case BUTTON_Right_Arrow_CONST:
		return make_pair<bool,int> (false,XK_Right);
	case BUTTON_Enter_CONST:
		return make_pair<bool,int> (false,XK_Return);
	case BUTTON_F1_CONST:
		return make_pair<bool,int> (false,XK_F1);
	case BUTTON_F2_CONST:
		return make_pair<bool,int> (false,XK_F2);
	case BUTTON_F3_CONST:
		return make_pair<bool,int> (false,XK_F3);
	case BUTTON_F4_CONST:
		return make_pair<bool,int> (false,XK_F4);
	case BUTTON_F5_CONST:
		return make_pair<bool,int> (false,XK_F5);
	case BUTTON_Back_CONST:
		return make_pair<bool,int> (false,XK_BackSpace);
	case BUTTON_space_CONST:
		return make_pair<bool,int> (false,XK_space);
	case BUTTON_comma_CONST:
		return make_pair<bool,int> (false,XK_comma);
	case BUTTON_period_CONST:
		return make_pair<bool,int> (false,XK_period);
	}
	return make_pair<bool,int> (false,0);
}

#endif
