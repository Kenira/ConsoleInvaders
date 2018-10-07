#ifndef KENIRA_MATHS_HEADER
#define KENIRA_MATHS_HEADER

#define NOMINMAX  // needed in main project file to get rid of error with std::min

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <array>
#include <random>


//TODO: - convert F_Hamilton_Product to using vectors

namespace kenira
{
namespace maths
{



struct Vec2i
{
	int x;
	int y;

	Vec2i operator+(const Vec2i& right) const
	{
		Vec2i temp;
		temp.x = this->x + right.x;
		temp.y = this->y + right.y;
		return temp;
	}

	Vec2i operator-(const Vec2i& right) const
	{
		Vec2i temp;
		temp.x = this->x - right.x;
		temp.y = this->y - right.y;
		return temp;
	}

	Vec2i& operator+=(const Vec2i& right)
	{
		this->x += right.x;
		this->y += right.y;
		return *this;
	}

	Vec2i& operator-=(const Vec2i& right)
	{
		this->x -= right.x;
		this->y -= right.y;
		return *this;
	}

	Vec2i operator*(const Vec2i& right) const
	{
		Vec2i temp;
		temp.x = this->x * right.x;
		temp.y = this->y * right.y;
		return temp;
	}

	Vec2i()
	{
		x = 0;
		y = 0;
	}

	Vec2i(int _x, int _y)
	{
		x = _x;
		y = _y;
	}


};

int random_int(int lower_bound, int higher_bound)
{
	std::random_device rdev;
	std::mt19937 mt_generator(rdev());
	std::uniform_int_distribution<> distribution(lower_bound, higher_bound);
	return distribution(mt_generator);
}


void vector_cross_product(double* w, double* u, double* v)
{
	w[0] = u[1] * v[2] - u[2] * v[1];
	w[1] = u[2] * v[0] - u[0] * v[2];
	w[2] = u[0] * v[1] - u[1] * v[0];
}

void vector_dot_product(double &w, double* u, double* v)				// overloaded dot product function: for writing dot product in variable
{
	w = u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

double vector_dot_product(double* u, double* v)						// or for directly returning value
{
	return (u[0] * v[0] + u[1] * v[1] + u[2] * v[2]);
}

#ifdef __MPREAL_H__
mpfr::mpreal vector_dot_product(mpfr::mpreal* u, double* v)			// or for directly returning value with mpreal
{
	return (u[0] * v[0] + u[1] * v[1] + u[2] * v[2]);
}
#endif

void matrix_vector_multiplication(double* w, double(&v)[3][3], double* u)
{
	w[0] = v[0][0] * u[0] + v[0][1] * u[1] + v[0][2] * u[2];
	w[1] = v[1][0] * u[0] + v[1][1] * u[1] + v[1][2] * u[2];
	w[2] = v[2][0] * u[0] + v[2][1] * u[1] + v[2][2] * u[2];
}

void hamilton_product(double* p_new, double* p, int dim_p, double *q, int dim_q)				// for multiplying 2 quaternions or 3D vectors converted
{																								// into quaternions, or a combination of quaternion and vector
	if (dim_p < 3 || dim_p > 4 || dim_q < 3 || dim_q > 4)												// if neither 3D nor 4D vector, error
	{
		std::cout << "CRITICAL ERROR: Neither 3D nor 4D vector given to Hamilton Product!" << std::endl;
	}
	else if ((dim_p == 3) && (dim_q == 3))
	{
		double p_[4] = { 0,p[0],p[1],p[2] };
		double q_[4] = { 0,q[0],q[1],q[2] };
		p_new[0] = p_[0] * q_[0] - p_[1] * q_[1] - p_[2] * q_[2] - p_[3] * q_[3];
		p_new[1] = p_[0] * q_[1] + p_[1] * q_[0] + p_[2] * q_[3] - p_[3] * q_[2];
		p_new[2] = p_[0] * q_[2] - p_[1] * q_[3] + p_[2] * q_[0] + p_[3] * q_[1];
		p_new[3] = p_[0] * q_[3] + p_[1] * q_[2] - p_[2] * q_[1] + p_[3] * q_[0];
		//std::cout << "Hamilton product: p and q were both 3D." << std::endl;
	}
	else if (dim_p == 3)																					// if p only 3D vector, make quaternion with real coordinate = 0
	{
		double p_[4] = { 0,p[0],p[1],p[2] };
		p_new[0] = p_[0] * q[0] - p_[1] * q[1] - p_[2] * q[2] - p_[3] * q[3];
		p_new[1] = p_[0] * q[1] + p_[1] * q[0] + p_[2] * q[3] - p_[3] * q[2];
		p_new[2] = p_[0] * q[2] - p_[1] * q[3] + p_[2] * q[0] + p_[3] * q[1];
		p_new[3] = p_[0] * q[3] + p_[1] * q[2] - p_[2] * q[1] + p_[3] * q[0];
		//std::cout << "Hamilton product: p was 3D." << std::endl;
	}
	else if (dim_q == 3)																				// if q only 3D vector, make quaternion with real coordinate = 0
	{
		double q_[4] = { 0,q[0],q[1],q[2] };
		p_new[0] = p[0] * q_[0] - p[1] * q_[1] - p[2] * q_[2] - p[3] * q_[3];
		p_new[1] = p[0] * q_[1] + p[1] * q_[0] + p[2] * q_[3] - p[3] * q_[2];
		p_new[2] = p[0] * q_[2] - p[1] * q_[3] + p[2] * q_[0] + p[3] * q_[1];
		p_new[3] = p[0] * q_[3] + p[1] * q_[2] - p[2] * q_[1] + p[3] * q_[0];
		//std::cout << "Hamilton product: q was 3D." << std::endl;
	}
	else																								// if both quaternions (4D), directly do hamilton product
	{
		p_new[0] = p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3];
		p_new[1] = p[0] * q[1] + p[1] * q[0] + p[2] * q[3] - p[3] * q[2];
		p_new[2] = p[0] * q[2] - p[1] * q[3] + p[2] * q[0] + p[3] * q[1];
		p_new[3] = p[0] * q[3] + p[1] * q[2] - p[2] * q[1] + p[3] * q[0];
		//std::cout << "Hamilton product: p and q 4D." << std::endl;
	}
}

void transform_global_local(double* local, double* global, double* r_c_up, double* r_c_star, double* r_c_att)
{
	//std::cout << "gl to loc, mag before: " << sqrt(pow(global[0],2) + pow(global[1],2) + pow(global[2],2)) << std::endl;
	local[0] = vector_dot_product(global, r_c_up);					// projection of global vector onto local x-axis (in global coordinates)
	local[1] = vector_dot_product(global, r_c_star);					// projection of global vector onto local y-axis (in global coordinates)
	local[2] = vector_dot_product(global, r_c_att);						// projection of global vector onto local z-axis (in global coordinates)
	//std::cout << "gl to loc, mag after: " << sqrt(pow(local[0],2) + pow(local[1],2) + pow(local[2],2)) << std::endl;
	/*std::cout << "global: " << global[0] << " / " << global[1] << " / " << global[2] << "\tloc: " << local[0] << " / " << local[1] << " / " << local[2] << std::endl;
	std::cout << "up: " << r_c_up[0] << " / " << r_c_up[1] << " / " << r_c_up[2] << std::endl;
	std::cout << "star: " << r_c_star[0] << " / " << r_c_star[1] << " / " << r_c_star[2] << std::endl;
	std::cout << "att: " << r_c_att[0] << " / " << r_c_att[1] << " / " << r_c_att[2] << std::endl;*/
	//char derpy; std::cin >> derpy;
}

void transform_local_global(double* global, double* local, double* r_c_up, double* r_c_star, double* r_c_att)
{
	//std::cout << "loc to gl, mag before: " << sqrt(pow(local[0],2) + pow(local[1],2) + pow(local[2],2)) << std::endl;
	global[0] = local[0] * r_c_up[0] + local[1] * r_c_star[0] + local[2] * r_c_att[0];			// global_vec = local_vec[0] * craft_up_vec + local_vec[1] * craft_star_vec + local_vec[2] * r_c_att_vec
	global[1] = local[0] * r_c_up[1] + local[1] * r_c_star[1] + local[2] * r_c_att[1];			// since craft_up_vec is the local x-axis, starboard the y-axis and r_c_att the z axis
	global[2] = local[0] * r_c_up[2] + local[1] * r_c_star[2] + local[2] * r_c_att[2];			// and local_vec[0] the magnitude of force in x-direction and so on.
	//std::cout << "loc to gl, mag after: " << sqrt(pow(global[0],2) + pow(global[1],2) + pow(global[2],2)) << std::endl;
}

void rotate_quat(double* result, double* input, double* delta_rot, double* r_c_up, double* r_c_star, double* r_c_att)
{
	double sin_half_alpha = sin(delta_rot[0] / 2);																			// half angles of rotation for quaternions for x, y and z axes
	double sin_half_beta = sin(delta_rot[1] / 2);
	double sin_half_gamma = sin(delta_rot[2] / 2);

	double quart_rot_x[4] = { cos(delta_rot[0] / 2), r_c_up[0] * sin_half_alpha, r_c_up[1] * sin_half_alpha, r_c_up[2] * sin_half_alpha };				// rotation quaternions for x, y and z axes
	double quart_rot_y[4] = { cos(delta_rot[1] / 2), r_c_star[0] * sin_half_beta, r_c_star[1] * sin_half_beta, r_c_star[2] * sin_half_beta };
	double quart_rot_z[4] = { cos(delta_rot[2] / 2), r_c_att[0] * sin_half_gamma, r_c_att[1] * sin_half_gamma, r_c_att[2] * sin_half_gamma };

	double abs_quart_rot_x = sqrt(pow(quart_rot_x[0], 2) + pow(quart_rot_x[1], 2) + pow(quart_rot_x[2], 2) + pow(quart_rot_x[3], 2));	// magnitude of quaternions to normalize them
	double abs_quart_rot_y = sqrt(pow(quart_rot_y[0], 2) + pow(quart_rot_y[1], 2) + pow(quart_rot_y[2], 2) + pow(quart_rot_y[3], 2));
	double abs_quart_rot_z = sqrt(pow(quart_rot_z[0], 2) + pow(quart_rot_z[1], 2) + pow(quart_rot_z[2], 2) + pow(quart_rot_z[3], 2));

	quart_rot_x[0] = quart_rot_x[0] / abs_quart_rot_x;
	quart_rot_x[1] = quart_rot_x[1] / abs_quart_rot_x;
	quart_rot_x[2] = quart_rot_x[2] / abs_quart_rot_x;
	quart_rot_x[3] = quart_rot_x[3] / abs_quart_rot_x;
	quart_rot_y[0] = quart_rot_y[0] / abs_quart_rot_y;
	quart_rot_y[1] = quart_rot_y[1] / abs_quart_rot_y;
	quart_rot_y[2] = quart_rot_y[2] / abs_quart_rot_y;
	quart_rot_y[3] = quart_rot_y[3] / abs_quart_rot_y;
	quart_rot_z[0] = quart_rot_z[0] / abs_quart_rot_z;
	quart_rot_z[1] = quart_rot_z[1] / abs_quart_rot_z;
	quart_rot_z[2] = quart_rot_z[2] / abs_quart_rot_z;
	quart_rot_z[3] = quart_rot_z[3] / abs_quart_rot_z;

	double quart_rot[4] = {};
	double quart_temp[4] = {};
	double quart_temp2[4] = {};

	hamilton_product(quart_temp, quart_rot_x, 4, quart_rot_y, 4);
	hamilton_product(quart_rot, quart_temp, 4, quart_rot_z, 4);											// total rotation quaternion = rot_x * rot_y * rot_z

	//std::cout << "quart rot: " << quart_rot[0] << " / " << quart_rot[1] << " / " << quart_rot[2] << " / " << quart_rot[3] << std::endl;

	double quart_rot_inv[4] = { quart_rot[0], -quart_rot[1], -quart_rot[2], -quart_rot[3] };					// inverse just has negative complex vector because it's unit (CHECK MAGNITUDE, FOR ERRORS)

	hamilton_product(quart_temp, quart_rot, 4, input, 3);													// rotate r_c_att
	hamilton_product(quart_temp2, quart_temp, 4, quart_rot_inv, 4);
	result[0] = quart_temp2[1];																				// 3D vector of quaternion is in elements 2 to 4, equivalent to indices 1 to 3
	result[1] = quart_temp2[2];
	result[2] = quart_temp2[3];
}

void reorthonormalization(double* r_c_att, double* r_c_star, double* r_c_up)				// Gram-Schmidt process
{
	double temp = sqrt(pow(r_c_att[0], 2) + pow(r_c_att[1], 2) + pow(r_c_att[2], 2));					// renormalize r_c_att by dividing by its magnitude
	r_c_att[0] = r_c_att[0] / temp;
	r_c_att[1] = r_c_att[1] / temp;
	r_c_att[2] = r_c_att[2] / temp;


	temp = vector_dot_product(r_c_star, r_c_att) / vector_dot_product(r_c_att, r_c_att);		// calculate projection of r_c_star on r_c_att
	double star_proj[3] = { r_c_att[0] * temp, r_c_att[1] * temp, r_c_att[2] * temp };

	r_c_star[0] = r_c_star[0] - star_proj[0];														// subtract projection for new orthogonal vector
	r_c_star[1] = r_c_star[1] - star_proj[1];
	r_c_star[2] = r_c_star[2] - star_proj[2];

	temp = sqrt(pow(r_c_star[0], 2) + pow(r_c_star[1], 2) + pow(r_c_star[2], 2));						// normalize, r_c_star is now orthonormal on r_c_att again
	r_c_star[0] = r_c_star[0] / temp;
	r_c_star[1] = r_c_star[1] / temp;
	r_c_star[2] = r_c_star[2] / temp;


	vector_cross_product(r_c_up, r_c_star, r_c_att);												// get r_c_up which is star cross forward (r_c_att)

	temp = sqrt(pow(r_c_up[0], 2) + pow(r_c_up[1], 2) + pow(r_c_up[2], 2));							// no real need for normalizing up again since r_c_att and r_c_star were both normalized
	//std::cout << "temp: " << temp << std::endl;
	//r_c_up[0] = r_c_up[0] / temp;
	//r_c_up[1] = r_c_up[1] / temp;
	//r_c_up[2] = r_c_up[2] / temp;
}

double safe_acos(double variable)
{
	if (variable > 1.0001 || variable < -1.0001)
	{
		std::cout << "ERROR: Gave acos function value with magnitude greater than one." << std::endl;
	}
	else if (variable > 1)													// avoid acos of number out of bounds due to floating point errors
	{
		variable = 1;
	}
	else if (variable < -1)
	{
		variable = -1;
	}
	return acos(variable);
}

#ifdef __MPREAL_H__
mpfr::mpreal kepler_c(mpfr::mpreal z)														// function to determine x for predicting new orbit position (Kepler problem, see gravity function in Celestial_Bodies.h)
{
	if (abs(z) > 1E-10)
		return ((1 - cos(sqrt(z))) / z);

	else
		return 0.5;
}

mpfr::mpreal kepler_s(mpfr::mpreal z)														// function to determine x for predicting new orbit position (Kepler problem, see gravity function in Celestial_Bodies.h)
{
	if (abs(z) > 1E-10)
		return ((sqrt(z) - sin(sqrt(z))) / sqrt(pow(z, 3)));

	else
		return 1 / 6;
}

void kepler_problem(mpfr::mpreal* pos_new, double* vel_new, mpfr::mpreal* pos, double* vel_orb, double t, double sma, double mu, double dist)

	// KEPLER PROBLEM, see "Fundamentals of Astrodynamics" for more details and how all the used equations are derived

{																	// STEP 1: FIND x, WITH dx/dt = sqrt(mu) / r
	mpfr::mpreal x = 1;														// start off with x = 1 as some starting point (TODO: SET TO SOMETHING MORE REASONABLE)
	mpfr::mpreal z = pow(x, 2) / sma;										// initialize z = x^2 / a
	//double t0 = 0;														// set start time t0 to 0 since time after periapsis does not matter (?)
	double tn = 0;															// initialize tn, the time that is calculated with trial value x, as 0
	double sq_mu = sqrt(mu);
	mpfr::mpreal vec_div_mu = vector_dot_product(pos, vel_orb) / sq_mu;
	mpfr::mpreal dt_dx = (pow(x, 2)*kepler_c(z) + vec_div_mu * x * (1 - z * kepler_s(z)) + dist * (1 - z * kepler_c(z))) / sq_mu;
	double count = 0;

	while (abs(1 - t / tn) > 1E-6)											// while difference between target time and time calculated with trial value x big
	{
		x = x + (t - tn) / dt_dx;
		z = pow(x, 2) / sma;
		dt_dx = (pow(x, 2)*kepler_c(z) + vec_div_mu * x * (1 - z * kepler_s(z)) + dist * (1 - z * kepler_c(z))) / sq_mu;
		tn = (pow(x, 3) * kepler_s(z) + vec_div_mu * pow(x, 2) * kepler_c(z) + dist * x * (1 - z * kepler_s(z))).toDouble();
		count++;
		std::cout << "x: " << x << "\tcount: " << count << std::endl;
		if (count > 1E3)
		{
			std::cout << "More than 1000 iterations to determine x. Break loop.\nx = " << x << "\tt - tn = " << t - tn << std::endl;
			break;
		}
	}
	std::cout << "count: " << count << "\tx: " << x << std::endl;
	char derpy; std::cin >> derpy;
	// GOT X. NOW NEED f AND g FOR NEW POSITION AND VELOCITY
	mpfr::mpreal f = 1 - pow(x, 2) / dist * kepler_c(z);
	mpfr::mpreal g = tn - pow(x, 3) / sq_mu * kepler_s(z);

	pos_new[0] = f * pos[0] + g * vel_orb[0];												// r = f * r0 + g * v0
	pos_new[1] = f * pos[1] + g * vel_orb[1];
	pos_new[2] = f * pos[2] + g * vel_orb[2];

	double dist_new_d = sqrt(pow(pos_new[0], 2) + pow(pos_new[1], 2) + pow(pos_new[2], 2)).toDouble();

	double f_dot = (sq_mu / (dist * dist_new_d) * x * (z * kepler_s(z) - 1)).toDouble();
	double g_dot = (1 - pow(x, 2) / dist_new_d * kepler_c(z)).toDouble();

	double should_be_one = (f * g_dot - f_dot * g).toDouble();
	std::cout << "should be one: " << should_be_one << std::endl;

	vel_new[0] = (f_dot * pos[0] + g_dot * vel_orb[0]).toDouble();										// v = f_dot * r0 + g_dot * v0
	vel_new[1] = (f_dot * pos[1] + g_dot * vel_orb[1]).toDouble();
	vel_new[2] = (f_dot * pos[2] + g_dot * vel_orb[2]).toDouble();
}

// RUNGE-KUTTA 4 FOR VARIABLE ACCELERATION
void integrate_motion_rk4(mpfr::mpreal* pos_new, double* vel_new, mpfr::mpreal* pos, double* vel, double interval, double* acc_pert, double mu, double* pos_par, double* vel_par, double dist)
{
	double pos_d[3] = { pos[0].toDouble(), pos[1].toDouble(), pos[2].toDouble() };
	//std::cout << "RK acc pert: " << acc_pert[0] << " / " << acc_pert[1] << " / " << acc_pert[2] << "\tmu: " << mu << "\npos_d: " << pos_d[0] << " / " << pos_d[1] << "\tdist: " << dist << std::endl;
	double par_delta_pos_half[3] = { 1 / 2 * interval * vel_par[0], 1 / 2 * interval * vel_par[1], 1 / 2 * interval * vel_par[2] };

	double dir[3] = { pos_par[0] - pos_d[0], pos_par[1] - pos_d[1], pos_par[2] - pos_d[2] };
	double Kv_1[3] = { acc_pert[0] + mu * dir[0] / pow(dist,3), acc_pert[1] + mu * dir[1] / pow(dist,3), acc_pert[2] + mu * dir[2] / pow(dist,3) };
	double Kr_1[3] = { vel[0], vel[1], vel[2] };
	//std::cout << "Kv_1: " << Kv_1[0] << " / " << Kv_1[1] << " / " << Kv_1[2] << std::endl;
	//std::cout << "Kr_1: " << Kr_1[0] << " / " << Kr_1[1] << " / " << Kr_1[2] << std::endl;

	dir[0] = dir[0] + par_delta_pos_half[0] - 1 / 2 * interval * Kr_1[0];															// minus sign because dir = pos_par - pos.
	dir[1] = dir[1] + par_delta_pos_half[1] - 1 / 2 * interval * Kr_1[1];															// dir = pos_par - (pos + 1/2 * interval * Kr_1)
	dir[2] = dir[2] + par_delta_pos_half[2] - 1 / 2 * interval * Kr_1[2];
	double dist_temp_sq = pow(dir[0], 2) + pow(dir[1], 2) + pow(dir[2], 2);
	//std::cout << "dist_temp_sq: " << dist_temp_sq << std::endl;
	double Kv_2[3] = { acc_pert[0] + mu * (dir[0]) / pow(dist_temp_sq, 1.5), acc_pert[1] + mu * (dir[1]) / pow(dist_temp_sq, 1.5),
						acc_pert[2] + mu * (dir[2]) / pow(dist_temp_sq, 1.5) };
	double Kr_2[3] = { vel[0] + 1 / 2 * interval * Kv_1[0], vel[1] + 1 / 2 * interval * Kv_1[1], vel[2] + 1 / 2 * interval * Kv_1[2] };
	//std::cout << "Kv_2: " << Kv_2[0] << " / " << Kv_2[1] << " / " << Kv_2[2] << std::endl;std::cout << "Kr_2: " << Kr_2[0] << " / " << Kr_2[1] << " / " << Kr_2[2] << std::endl;

	dir[0] = pos_par[0] + par_delta_pos_half[0] - (pos_d[0] + 1 / 2 * interval * Kr_2[0]);
	dir[1] = pos_par[1] + par_delta_pos_half[1] - (pos_d[1] + 1 / 2 * interval * Kr_2[1]);
	dir[2] = pos_par[2] + par_delta_pos_half[2] - (pos_d[2] + 1 / 2 * interval * Kr_2[2]);
	dist_temp_sq = pow(dir[0], 2) + pow(dir[1], 2) + pow(dir[2], 2);
	//std::cout << "dist_temp_sq: " << dist_temp_sq << std::endl;
	double Kv_3[3] = { acc_pert[0] + mu * (dir[0]) / pow(dist_temp_sq, 1.5), acc_pert[1] + mu * (dir[1]) / pow(dist_temp_sq, 1.5),
						acc_pert[2] + mu * (dir[2]) / pow(dist_temp_sq, 1.5) };
	double Kr_3[3] = { vel[0] + 1 / 2 * interval * Kv_2[0], vel[1] + 1 / 2 * interval * Kv_2[1], vel[2] + 1 / 2 * interval * Kv_2[2] };
	//std::cout << "Kv_3: " << Kv_3[0] << " / " << Kv_3[1] << " / " << Kv_3[2] << std::endl;std::cout << "Kr_3: " << Kr_3[0] << " / " << Kr_3[1] << " / " << Kr_3[2] << std::endl;

	dir[0] = pos_par[0] + 2 * par_delta_pos_half[0] - (pos_d[0] + interval * Kr_3[0]);
	dir[1] = pos_par[1] + 2 * par_delta_pos_half[1] - (pos_d[1] + interval * Kr_3[1]);
	dir[2] = pos_par[2] + 2 * par_delta_pos_half[2] - (pos_d[2] + interval * Kr_3[2]);
	dist_temp_sq = pow(dir[0], 2) + pow(dir[1], 2) + pow(dir[2], 2);
	//std::cout << "dist_temp_sq: " << dist_temp_sq << std::endl;
	double Kv_4[3] = { acc_pert[0] + mu * (dir[0]) / pow(dist_temp_sq, 1.5), acc_pert[1] + mu * (dir[1]) / pow(dist_temp_sq, 1.5),
						acc_pert[2] + mu * (dir[2]) / pow(dist_temp_sq, 1.5) };
	double Kr_4[3] = { vel[0] + interval * Kv_3[0], vel[1] + interval * Kv_3[1], vel[2] + interval * Kv_3[2] };
	//std::cout << "Kv_4: " << Kv_4[0] << " / " << Kv_4[1] << " / " << Kv_4[2] << std::endl;std::cout << "Kr_4: " << Kr_4[0] << " / " << Kr_4[1] << " / " << Kr_4[2] << std::endl;

	vel_new[0] = vel[0] + interval / 6 * (Kv_1[0] + 2 * Kv_2[0] + 2 * Kv_3[0] + Kv_4[0]);
	vel_new[1] = vel[1] + interval / 6 * (Kv_1[1] + 2 * Kv_2[1] + 2 * Kv_3[1] + Kv_4[1]);
	vel_new[2] = vel[2] + interval / 6 * (Kv_1[2] + 2 * Kv_2[2] + 2 * Kv_3[2] + Kv_4[2]);

	pos_new[0] = pos[0] + interval / 6 * (Kr_1[0] + 2 * Kr_2[0] + 2 * Kr_3[0] + Kr_4[0]);
	pos_new[1] = pos[1] + interval / 6 * (Kr_1[1] + 2 * Kr_2[1] + 2 * Kr_3[1] + Kr_4[1]);
	pos_new[2] = pos[2] + interval / 6 * (Kr_1[2] + 2 * Kr_2[2] + 2 * Kr_3[2] + Kr_4[2]);
	//std::cout << "pos_new: " << pos_new[0] << " / " << pos_new[1] << std::endl;
}

// SIMPLE EULER FOR CONSTANT ACCELERATION
void integrate_motion_euler(mpfr::mpreal* pos_new, double* vel_new, mpfr::mpreal* pos, double* vel, double interval, double* acc)					// without changing acceleration
{
	double pos_d[3] = { pos[0].toDouble(), pos[1].toDouble(), pos[2].toDouble() };

	vel_new[0] = vel[0] + interval * acc[0];
	vel_new[1] = vel[1] + interval * acc[1];
	vel_new[2] = vel[2] + interval * acc[2];

	pos_new[0] = pos[0] + vel[0] * interval + 1 / 2 * acc[0] * pow(interval, 2);
	pos_new[1] = pos[1] + vel[1] * interval + 1 / 2 * acc[1] * pow(interval, 2);
	pos_new[2] = pos[2] + vel[2] * interval + 1 / 2 * acc[2] * pow(interval, 2);
}
#else
double kepler_c(double z)														// function to determine x for predicting new orbit position (Kepler problem, see gravity function in Celestial_Bodies.h)
{
	if (abs(z) > 1E-10)
		return ((1 - cos(sqrt(z))) / z);

	else
		return 0.5;
}

double kepler_s(double z)														// function to determine x for predicting new orbit position (Kepler problem, see gravity function in Celestial_Bodies.h)
{
	if (abs(z) > 1E-10)
		return ((sqrt(z) - sin(sqrt(z))) / sqrt(pow(z, 3)));

	else
		return 1 / 6;
}

void kepler_problem(double* pos_new, double* vel_new, double* pos, double* vel_orb, double t, double sma, double mu, double dist)

	// KEPLER PROBLEM, see "Fundamentals of Astrodynamics" for more details and how all the used equations are derived

{																	// STEP 1: FIND x, WITH dx/dt = sqrt(mu) / r
	double x = 1;														// start off with x = 1 as some starting point (TODO: SET TO SOMETHING MORE REASONABLE)
	double z = pow(x, 2) / sma;										// initialize z = x^2 / a
	//double t0 = 0;														// set start time t0 to 0 since time after periapsis does not matter (?)
	double tn = 0;															// initialize tn, the time that is calculated with trial value x, as 0
	double sq_mu = sqrt(mu);
	double vec_div_mu = vector_dot_product(pos, vel_orb) / sq_mu;
	double dt_dx = (pow(x, 2)*kepler_c(z) + vec_div_mu * x * (1 - z * kepler_s(z)) + dist * (1 - z * kepler_c(z))) / sq_mu;
	double count = 0;

	while (abs(1 - t / tn) > 1E-6)											// while difference between target time and time calculated with trial value x big
	{
		x = x + (t - tn) / dt_dx;
		z = pow(x, 2) / sma;
		dt_dx = (pow(x, 2)*kepler_c(z) + vec_div_mu * x * (1 - z * kepler_s(z)) + dist * (1 - z * kepler_c(z))) / sq_mu;
		tn = (pow(x, 3) * kepler_s(z) + vec_div_mu * pow(x, 2) * kepler_c(z) + dist * x * (1 - z * kepler_s(z)));
		count++;
		std::cout << "x: " << x << "\tcount: " << count << std::endl;
		if (count > 1E3)
		{
			std::cout << "More than 1000 iterations to determine x. Break loop.\nx = " << x << "\tt - tn = " << t - tn << std::endl;
			break;
		}
	}
	std::cout << "count: " << count << "\tx: " << x << std::endl;
	char derpy; std::cin >> derpy;
	// GOT X. NOW NEED f AND g FOR NEW POSITION AND VELOCITY
	double f = 1 - pow(x, 2) / dist * kepler_c(z);
	double g = tn - pow(x, 3) / sq_mu * kepler_s(z);

	pos_new[0] = f * pos[0] + g * vel_orb[0];												// r = f * r0 + g * v0
	pos_new[1] = f * pos[1] + g * vel_orb[1];
	pos_new[2] = f * pos[2] + g * vel_orb[2];

	double dist_new_d = sqrt(pow(pos_new[0], 2) + pow(pos_new[1], 2) + pow(pos_new[2], 2));

	double f_dot = (sq_mu / (dist * dist_new_d) * x * (z * kepler_s(z) - 1));
	double g_dot = (1 - pow(x, 2) / dist_new_d * kepler_c(z));

	double should_be_one = (f * g_dot - f_dot * g);
	std::cout << "should be one: " << should_be_one << std::endl;

	vel_new[0] = (f_dot * pos[0] + g_dot * vel_orb[0]);										// v = f_dot * r0 + g_dot * v0
	vel_new[1] = (f_dot * pos[1] + g_dot * vel_orb[1]);
	vel_new[2] = (f_dot * pos[2] + g_dot * vel_orb[2]);
}

// RUNGE-KUTTA 4 FOR VARIABLE ACCELERATION
void integrate_motion_rk4(double* pos_new, double* vel_new, double* pos, double* vel, double interval, double* acc_pert, double mu, double* pos_par, double* vel_par, double dist)
{
	double pos_d[3] = { pos[0], pos[1], pos[2] };
	//std::cout << "RK acc pert: " << acc_pert[0] << " / " << acc_pert[1] << " / " << acc_pert[2] << "\tmu: " << mu << "\npos_d: " << pos_d[0] << " / " << pos_d[1] << "\tdist: " << dist << std::endl;
	double par_delta_pos_half[3] = { 1 / 2 * interval * vel_par[0], 1 / 2 * interval * vel_par[1], 1 / 2 * interval * vel_par[2] };

	double dir[3] = { pos_par[0] - pos_d[0], pos_par[1] - pos_d[1], pos_par[2] - pos_d[2] };
	double Kv_1[3] = { acc_pert[0] + mu * dir[0] / pow(dist,3), acc_pert[1] + mu * dir[1] / pow(dist,3), acc_pert[2] + mu * dir[2] / pow(dist,3) };
	double Kr_1[3] = { vel[0], vel[1], vel[2] };
	//std::cout << "Kv_1: " << Kv_1[0] << " / " << Kv_1[1] << " / " << Kv_1[2] << std::endl;
	//std::cout << "Kr_1: " << Kr_1[0] << " / " << Kr_1[1] << " / " << Kr_1[2] << std::endl;

	dir[0] = dir[0] + par_delta_pos_half[0] - 1 / 2 * interval * Kr_1[0];															// minus sign because dir = pos_par - pos.
	dir[1] = dir[1] + par_delta_pos_half[1] - 1 / 2 * interval * Kr_1[1];															// dir = pos_par - (pos + 1/2 * interval * Kr_1)
	dir[2] = dir[2] + par_delta_pos_half[2] - 1 / 2 * interval * Kr_1[2];
	double dist_temp_sq = pow(dir[0], 2) + pow(dir[1], 2) + pow(dir[2], 2);
	//std::cout << "dist_temp_sq: " << dist_temp_sq << std::endl;
	double Kv_2[3] = { acc_pert[0] + mu * (dir[0]) / pow(dist_temp_sq, 1.5), acc_pert[1] + mu * (dir[1]) / pow(dist_temp_sq, 1.5),
						acc_pert[2] + mu * (dir[2]) / pow(dist_temp_sq, 1.5) };
	double Kr_2[3] = { vel[0] + 1 / 2 * interval * Kv_1[0], vel[1] + 1 / 2 * interval * Kv_1[1], vel[2] + 1 / 2 * interval * Kv_1[2] };
	//std::cout << "Kv_2: " << Kv_2[0] << " / " << Kv_2[1] << " / " << Kv_2[2] << std::endl;std::cout << "Kr_2: " << Kr_2[0] << " / " << Kr_2[1] << " / " << Kr_2[2] << std::endl;

	dir[0] = pos_par[0] + par_delta_pos_half[0] - (pos_d[0] + 1 / 2 * interval * Kr_2[0]);
	dir[1] = pos_par[1] + par_delta_pos_half[1] - (pos_d[1] + 1 / 2 * interval * Kr_2[1]);
	dir[2] = pos_par[2] + par_delta_pos_half[2] - (pos_d[2] + 1 / 2 * interval * Kr_2[2]);
	dist_temp_sq = pow(dir[0], 2) + pow(dir[1], 2) + pow(dir[2], 2);
	//std::cout << "dist_temp_sq: " << dist_temp_sq << std::endl;
	double Kv_3[3] = { acc_pert[0] + mu * (dir[0]) / pow(dist_temp_sq, 1.5), acc_pert[1] + mu * (dir[1]) / pow(dist_temp_sq, 1.5),
						acc_pert[2] + mu * (dir[2]) / pow(dist_temp_sq, 1.5) };
	double Kr_3[3] = { vel[0] + 1 / 2 * interval * Kv_2[0], vel[1] + 1 / 2 * interval * Kv_2[1], vel[2] + 1 / 2 * interval * Kv_2[2] };
	//std::cout << "Kv_3: " << Kv_3[0] << " / " << Kv_3[1] << " / " << Kv_3[2] << std::endl;std::cout << "Kr_3: " << Kr_3[0] << " / " << Kr_3[1] << " / " << Kr_3[2] << std::endl;

	dir[0] = pos_par[0] + 2 * par_delta_pos_half[0] - (pos_d[0] + interval * Kr_3[0]);
	dir[1] = pos_par[1] + 2 * par_delta_pos_half[1] - (pos_d[1] + interval * Kr_3[1]);
	dir[2] = pos_par[2] + 2 * par_delta_pos_half[2] - (pos_d[2] + interval * Kr_3[2]);
	dist_temp_sq = pow(dir[0], 2) + pow(dir[1], 2) + pow(dir[2], 2);
	//std::cout << "dist_temp_sq: " << dist_temp_sq << std::endl;
	double Kv_4[3] = { acc_pert[0] + mu * (dir[0]) / pow(dist_temp_sq, 1.5), acc_pert[1] + mu * (dir[1]) / pow(dist_temp_sq, 1.5),
						acc_pert[2] + mu * (dir[2]) / pow(dist_temp_sq, 1.5) };
	double Kr_4[3] = { vel[0] + interval * Kv_3[0], vel[1] + interval * Kv_3[1], vel[2] + interval * Kv_3[2] };
	//std::cout << "Kv_4: " << Kv_4[0] << " / " << Kv_4[1] << " / " << Kv_4[2] << std::endl;std::cout << "Kr_4: " << Kr_4[0] << " / " << Kr_4[1] << " / " << Kr_4[2] << std::endl;

	vel_new[0] = vel[0] + interval / 6 * (Kv_1[0] + 2 * Kv_2[0] + 2 * Kv_3[0] + Kv_4[0]);
	vel_new[1] = vel[1] + interval / 6 * (Kv_1[1] + 2 * Kv_2[1] + 2 * Kv_3[1] + Kv_4[1]);
	vel_new[2] = vel[2] + interval / 6 * (Kv_1[2] + 2 * Kv_2[2] + 2 * Kv_3[2] + Kv_4[2]);

	pos_new[0] = pos[0] + interval / 6 * (Kr_1[0] + 2 * Kr_2[0] + 2 * Kr_3[0] + Kr_4[0]);
	pos_new[1] = pos[1] + interval / 6 * (Kr_1[1] + 2 * Kr_2[1] + 2 * Kr_3[1] + Kr_4[1]);
	pos_new[2] = pos[2] + interval / 6 * (Kr_1[2] + 2 * Kr_2[2] + 2 * Kr_3[2] + Kr_4[2]);
	//std::cout << "pos_new: " << pos_new[0] << " / " << pos_new[1] << std::endl;
}

// SIMPLE EULER FOR CONSTANT ACCELERATION
void integrate_motion_euler(double* pos_new, double* vel_new, double* pos, double* vel, double interval, double* acc)					// without changing acceleration
{
	double pos_d[3] = { pos[0], pos[1], pos[2] };

	vel_new[0] = vel[0] + interval * acc[0];
	vel_new[1] = vel[1] + interval * acc[1];
	vel_new[2] = vel[2] + interval * acc[2];

	pos_new[0] = pos[0] + vel[0] * interval + 1 / 2 * acc[0] * pow(interval, 2);
	pos_new[1] = pos[1] + vel[1] * interval + 1 / 2 * acc[1] * pow(interval, 2);
	pos_new[2] = pos[2] + vel[2] * interval + 1 / 2 * acc[2] * pow(interval, 2);
}
#endif

/*void F_Position_Velocity_RKF45(mpfr::mpreal* pos_new, double* vel_new, mpfr::mpreal* pos, double* vel, double interval, double* acc_pert, double mu, double dist)
{
	double pos_d[3] = {pos[0].toDouble(), pos[1].toDouble(), pos[2].toDouble()};
	double dist_temp_sq = 0;

	double Kv_1[3] = {interval*(acc_pert[0] + mu*pos_d[0]/pow(dist,3)), interval*(acc_pert[1] + mu*pos_d[1]/pow(dist,3)), interval*(acc_pert[2] + mu*pos_d[2]/pow(dist,3))};
	double Kr_1[3] = {interval*vel[0], interval*vel[1], interval*vel[2]};

	dist_temp_sq = pow(pos_d[0] + 1/4 * Kr_1[0], 2) + pow(pos_d[1] + 1/4 * Kr_1[1], 2) + pow(pos_d[2] + 1/4 * Kr_1[2], 2);

	double Kv_2[3] = {interval*(acc_pert[0] + mu*(pos_d[0] + 1/4 * Kr_1[0])/pow(dist_temp_sq, 1.5)), interval*(acc_pert[1] + mu*(pos_d[1] + 1/4 * Kr_1[1])/pow(dist_temp_sq, 1.5)),
						interval*(acc_pert[2] + mu*(pos_d[2] + 1/4 * Kr_1[2])/pow(dist_temp_sq, 1.5))};
	double Kr_2[3] = {interval*(vel[0] + 1/4 * Kv_1[0]), interval*(vel[1] + 1/4 * Kv_1[1]), interval*(vel[2] + 1/4 * Kv_1[2])};

	dist_temp_sq = pow(pos_d[0] + 3/32 * Kr_1[0] + 9/32 * Kr_2[0],2) + pow(pos_d[1] + 3/32 * Kr_1[1] + 9/32 * Kr_2[1],2) + pow(pos_d[2] + 3/32 * Kr_1[2] + 9/32 * Kr_2[2],2);

	double Kv_3[3] = {interval*(acc_pert[0] + mu*(pos_d[0] + 3/32 * Kr_1[0] + 9/32 * Kr_2[0])/pow(dist_temp_sq, 1.5)),
						interval*(acc_pert[1] + mu*(pos_d[1] + 3/32 * Kr_1[1] + 9/32 * Kr_2[1])/pow(dist_temp_sq, 1.5)),
						interval*(acc_pert[2] + mu*(pos_d[2] + 3/32 * Kr_1[2] + 9/32 * Kr_2[2])/pow(dist_temp_sq, 1.5))};
	double Kr_3[3] = {interval*(vel[0] + 3/32 * Kv_1[0] + 9/32 * Kv_2[0]), interval*(vel[1] + 3/32 * Kv_1[1] + 9/32 * Kv_2[1]), interval*(vel[2] + 3/32 * Kv_1[2] + 9/32 * Kv_2[2])};

	dist_temp_sq = pow(pos_d[0]

	double Kv_4[3] = {interval*(acc_pert[0] + mu*(
}*/

class frac
{
public:
	int num;
	int den;
	frac(int n, int d);
};

frac::frac(int n, int d)
{
	num = n;
	den = d;
}


template<class T>
std::vector<std::vector<T>> rotate_90d_r(std::vector<std::vector<T>> vv)
{
	std::vector<std::vector<T>> ret;
	for (int i = 0; i < vv[0].size(); i++)
	{
		std::vector<T> row;
		for (int j = vv.size() - 1; j >= 0; j--)
		{
			row.push_back(vv[j][i]);
		}
		ret.push_back(row);
	}
	return ret;
}

template<class T>
std::vector<std::vector<T>> rotate_90d_l(std::vector<std::vector<T>> vv)
{
	std::vector<std::vector<T>> ret;
	for (int i = vv[0].size() - 1; i >= 0; i--)
	{
		std::vector<T> row;
		for (int j = 0; j < vv.size(); j++)
		{
			row.push_back(vv[j][i]);
		}
		ret.push_back(row);
	}
	return ret;
}

std::pair<int, int> rotate_dir_90d_r(std::pair<int, int> p)
{
	if (p.first != 0)
	{
		p.second = -p.first;
		p.first = 0;
	}
	else
	{
		p.first = p.second;
		p.second = 0;
	}
	return p;
}

std::pair<int, int> rotate_dir_90d_l(std::pair<int, int> p)
{
	if (p.first != 0)
	{
		p.second = p.first;
		p.first = 0;
	}
	else
	{
		p.first = -p.second;
		p.second = 0;
	}
	return p;
}

class Primes
{
private:
	std::vector<unsigned long long> primes;				// found primes
	unsigned long long limit = 0;						// how far has been searched for primes

	std::experimental::filesystem::path p_primefile = "E:/C++/Primes/Primes.txt";

	int read_file()
	{
		if (!std::experimental::filesystem::exists(p_primefile))
		{
			std::cout << "ERROR in cPrimes.F_ReadFile: File path does not exist!" << std::endl;
			return 0;
		}
		std::vector<std::string> str = kenira::io::read_file_array(p_primefile);
		primes = kenira::io::convert_vstring_vull(std::vector<std::string>(str.begin() + 1, str.end()));
		limit = std::stoull(str[0]);

		//std::vector<std::string>(str.begin() + 1, str.end()).swap(str);
		return 1;
	}

	unsigned long long read_limit()
	{
		if (!std::experimental::filesystem::exists(p_primefile))
		{
			std::cout << "ERROR in cPrimes.F_ReadFile: File path does not exist!" << std::endl;
			return 0;
		}
		return std::stoull(kenira::io::read_file_line(p_primefile, 1));
	}

	int save_file()
	{
		std::vector<std::string> str;

		if (!std::experimental::filesystem::exists(p_primefile))
		{
			str.push_back(std::to_string(limit).append(" limit"));
			for (auto&& p : primes)
			{
				str.push_back(std::to_string(p));
			}
		}
		else
		{
			unsigned long long flimit = read_limit();

			size_t ind = primes.size();
			for (int i = 0; i < primes.size(); i++)
			{
				if (primes[i] > flimit)
				{
					ind = i;
					break;
				}
			}
			for (size_t i = ind; i < primes.size(); i++)
			{
				str.push_back(std::to_string(primes[i]));
			}
		}



		return kenira::io::write_array_file(p_primefile, str, "app");
	}

	//cPrimes(unsigned long long);

public:
	Primes()
	{
		primes.push_back(2);
		primes.push_back(3);
		limit = 3;
	}
	Primes(unsigned long long num)
	{
		primes.push_back(2);
		primes.push_back(3);
		limit = 3;
		calculate_primes(num);
	}

	int calculate_primes(unsigned long long num)
	{
		if (num < limit)
		{
			return 1;
		}

		std::vector<unsigned long long> nums;
		for (int i = 0; i <= num; i++)
		{
			nums.push_back(i);
		}

		for (int i = 2; i <= sqrt(double(num) + 0.1); i++)
		{
			for (int j = i * i; j <= num; j += i)
			{
				nums[j] = 0;
			}
		}

		for (unsigned long long i = limit + 1; i <= num; i++)
		{
			if (nums[i] != 0)
			{
				primes.push_back(nums[i]);
				//std::cout << nums[i] << std::endl;
			}
		}

		// mad calculations

		limit = num;

		save_file();
		return 1;
	}

	bool F_Is_Prime(unsigned long long num)
	{
		if (num > limit)
		{
			calculate_primes(num);
		}
		return std::find(primes.begin(), primes.end(), num) == primes.end();
	}
};

bool is_prime_simple(unsigned long long int num)
{
	if (num <= 1 || num % 2 == 0)
	{
		return false;
	}

	for (int i = 3; i <= sqrt(static_cast<double>(num) + 3); i = i + 2)
	{
		if (num%i == 0)
		{
			return 0;
		}
	}
	return 1;
};

bool prime_factors(int num, std::vector<int>* fac)
{
	bool found_factor = false;
	for (int i = 2; i <= num / 2 + 1; i++)
	{
		int temp = num;
		while (temp%i == 0 && is_prime_simple(i) == true)
		{
			fac->push_back(i);
			temp /= i;
			found_factor = true;
		}
	}
	std::sort(fac->begin(), fac->end());
	return found_factor;
}

int convert_vecvec_vec(std::vector<std::vector<int>> inp, std::vector<int>& out)
{
	for (auto&& i : inp)
	{
		if (i.size() != 1)
		{
			std::cout << "ERROR IN convert_vecvec_vec: size of vec<vec> != 1!" << std::endl;
			return 0;
		}
		out.push_back(i[0]);
	}
	return 1;
}

std::vector<int> convert_vecvec_vec(std::vector<std::vector<int>>& inp)
{
	std::vector<int> v;
	for (auto&& i : inp)
	{
		if (i.size() != 1)
		{
			std::cout << "ERROR IN convert_vecvec_vec: size of vec<vec> != 1!" << std::endl;
			return {};
		}
		v.push_back(i[0]);
	}
	return v;
}

int alphabetic_number(char ch)
{
	if ((int)ch >= 97)
	{
		if ((int)ch >= 123)
		{
			std::cout << "ERROR: (int)ch >= 123." << std::endl;
			return 0;
		}
		return (int)ch - 96;
	}
	else if ((int)ch >= 65)		// upper case letters: 65 to 90
	{
		if ((int)ch >= 91)
		{
			std::cout << "ERROR: 91 <= (int)ch <= 96." << std::endl;
			return 0;
		}
		return (int)ch - 64;
	}
	else
	{
		std::cout << "ERROR: (int)ch <= 64." << std::endl;
		return 0;
	}
};

char in_to_ascii(int num)
{
	if (num > 0)
	{
		if (num > 96)
		{
			if (num > 122)
			{
				std::cout << "ERROR in F_Value_To_ASCII: num > 122." << std::endl;
				return 0;
			}
			return char(num);
		}
		if (num > 64)
		{
			if (num > 91)
			{
				std::cout << "ERROR in F_Value_To_ASCII: num > 91." << std::endl;
				return 0;
			}
			return char(num + 64);
		}
	}
	else
	{
		return '\0';
	}
}

std::string knot_hash(std::string str)
{
	std::vector<int> iinp;
	for (size_t i = 0; i < str.size(); i++)
	{
		iinp.push_back(int(str[i]));
	}
	std::vector<int> app = { 17, 31, 73, 47, 23 };
	iinp.insert(iinp.end(), app.begin(), app.end());
	std::vector<int> list;

	for (int i = 0; i < 256; i++)
	{
		list.push_back(i);
	}
	int current_position = 0;
	int skip_size = 0;
	std::vector<int> sequence;

	for (int i = 0; i < 64; i++)
	{
		for (auto len : iinp)
		{
			if (len <= 1)
			{
				// do nothing
			}
			else if (current_position + len < list.size())
			{
				std::reverse(list.begin() + current_position, list.begin() + current_position + len);
			}
			else
			{
				std::vector<int>tlist = list;
				tlist.insert(tlist.end(), list.begin(), list.end());
				std::reverse(tlist.begin() + current_position, tlist.begin() + current_position + len);
				for (int j = 0; j < len; j++)
				{
					if (current_position + j < list.size())
					{
						list[current_position + j] = tlist[current_position + j];
					}
					else
					{
						list[current_position + j - list.size()] = tlist[current_position + j];
					}
				}
			}
			current_position = (current_position + len + skip_size) % list.size();
			skip_size++;
		}
	}

	std::stringstream ss;
	for (int i = 0; i < 16; i++)
	{
		int dense_hash = 0;
		for (int j = 0; j < 16; j++)
		{
			dense_hash ^= list[std::min(i * 16 + j, int(list.size()))];
		}
		ss << std::setbase(16) << std::setfill('0') << std::setw(2) << dense_hash;
	}

	return ss.str();
}


// double the size of a vector. (21 -> 41 etc, one extra for a clearly defined center)
int resize(std::vector<int>& v, int& pos, int factor)
{
	int new_size = (int)(v.size() * factor);
	std::vector<int> v2(new_size, 0);
	int shift = (int)(new_size - v.size()) / 2;
	std::copy(v.begin(), v.end(), v2.begin() + shift);
	pos += shift;
	v = v2;
	return 1;
}

int resize(std::vector<std::vector<int>>& vv, std::pair<int, int>& pos, std::pair<int, int> factor)
{
	int new_size[2] = { (int)vv.size() * factor.first, (int)vv[0].size() * factor.second };
	int first_size = (int)vv[0].size();
	int shift[2] = { (new_size[0] - (int)vv.size()) / 2, (new_size[1] - (int)vv[0].size()) / 2 };

	if (factor.second > 1)
	{
		for (auto&& v : vv)
		{
			if (v.size() != first_size)
			{
				std::cout << "ERROR in F_Resize: vectors in vec<vec> not all same size!" << '\n';
			}
			std::vector<int> v2(new_size[1], 0);
			std::copy(v.begin(), v.end(), v2.begin() + shift[1]);
			v2.swap(v);
		}
		pos.second += shift[1];
	}

	if (factor.first > 1)
	{
		std::vector<std::vector<int>> pad(shift[0], std::vector<int>(new_size[1]));
		vv.insert(vv.begin(), pad.begin(), pad.end());
		pad = std::vector<std::vector<int>>(new_size[0] - new_size[0] / factor.first - shift[0], std::vector<int>(new_size[1]));
		vv.insert(vv.end(), pad.begin(), pad.end());
		pos.first += shift[0];
	}
	return 1;
}

std::string string_hex_to_bit(std::string shex)
{
	std::string sbit = "";
	for (int i = 0; i < shex.size(); i++)
	{
		sbit.append(std::bitset<4>(std::stoi(shex.substr(i, 1), nullptr, 16)).to_string());
	}
	return sbit;
}

/*std::string F_String_Bit_To_Hex(std::string str)
{
	if (str.size() % 4 != 0)
	{
		std::cout << "ERROR in F_String_Bit_To_Hex: String not size % 4 == 0" << '\n';
	}

	std::string shex = "";
	for (int i = 0; i < str.size() / 4; i++)
	{
		shex.append().to_string());
	}
	return shex;
}*/

std::string string_to_bit(std::string hash)
{
	std::string sbit = "";
	for (int i = 0; i < hash.size(); i++)
	{
		sbit.append(std::bitset<7>(int(hash[i])).to_string());
	}
	return sbit;
}

unsigned md5_hash_rotate(unsigned x, unsigned c)
{
	return (x << c) | (x >> (32 - c));
}

std::string md5_hash(std::string str)
{
	unsigned s[64] = { 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
					5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
					4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
					6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 };

	unsigned K[64] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
						0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
						0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
						0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
						0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
						0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
						0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
						0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
						0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
						0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
						0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
						0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
						0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
						0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
						0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
						0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

	unsigned a0 = 0x67452301;   //A
	unsigned b0 = 0xefcdab89;   //B
	unsigned c0 = 0x98badcfe;   //C
	unsigned d0 = 0x10325476;   //D

	std::string bstr = string_to_bit(str) + "1";
	std::string ts;
	ts.assign(bstr.size() % 512 <= 448 ? 448 - (bstr.size() % 512) : 512 - (bstr.size() % 512 - 448), '0'); // pad to 448 bits on the last 512 bit chunk
	//ts.assign(448 - ((bstr.size() % 512) - 64), '0');
	bstr.append(ts);
	ts = std::bitset<64>(str.size() % SIZE_MAX).to_string();//uint64_t(pow(2, 64) + 0.5)).to_string();
	bstr.append(ts);

	for (int ch = 0; ch < bstr.size() / 512; ch++)
	{
		std::array<unsigned, 16> M;
		for (int j = 0; j < 16; ++j)
		{
			//std::cout << bstr.substr(j * 32, 32) << '\n';
			//std::cout << std::stoul(bstr.substr(j * 32, 32), nullptr, 2) << '\n';
			M[j] = unsigned(std::stoul(bstr.substr(j * 32, 32), nullptr, 2));
			//std::cout << std::stoul(bstr.substr(j * 32, 32), nullptr, 2) << '\n';
		}
		unsigned A = a0;
		unsigned B = b0;
		unsigned C = c0;
		unsigned D = d0;

		for (int i = 0; i < 64; i++)
		{
			unsigned F, g;
			if (i < 16)
			{
				//F = (B & C) | ((~B) & D);
				F = D ^ (B & (C ^ D));
				g = i;
			}
			else if (i < 32)
			{
				//F = (D & B) | ((~D) & C);
				F = C ^ (D & (B ^ C));
				g = (5 * i + 1) % 16;
			}
			else if (i < 48)
			{
				F = B ^ C ^ D;
				g = (3 * i + 5) % 16;
			}
			else
			{
				F = C ^ (B | (~D));
				g = (7 * i) % 16;
			}
			F += A + K[i] + M[g];
			A = D;
			D = C;
			C = B;
			B += md5_hash_rotate(F, s[i]);
		}

		a0 += A;
		b0 += B;
		c0 += C;
		d0 += D;
	}
	std::stringstream ss;
	//std::cout << std::hex << a0 << ", " << b0 << ", " << c0 << ", " << d0 << '\n';
	//int derp = 62352;
	std::cout << std::setbase(16) << a0;
	//ss << std::setbase(16) << int(derp);
	//ss << std::setbase(16) << int(a0) << int(b0) << int(c0) << int(d0);
	ss << std::setbase(16) << a0 << b0 << c0 << d0;
	//ss << a0 << b0 << c0 << d0;
	return ss.str();
}


}
}
#endif