#ifndef __PENDULUMS_H__
#define __PENDULUMS_H__

#include <vector>
#include <cmath>
#include <string>

namespace pendulums{
	const static double PI = 4*std::atan(1.0);
	const static double g = 2.0;//9.8; // gravitational acceleraion
	using std::sin;
	using std::cos;

	//pendState:     struct for variables for pendulums (theta x3, differential of theta x3)
	//pendStateDiff: struct for differential of pendState
	struct pendStateDiff;
	struct pendState{
		double theta[3];
		double dtheta[3];
		pendState();
		void addDiff(const pendState p, const pendStateDiff pd, double r);
		void adjust();
	};
	struct pendStateDiff{
		double diff[6];
		pendStateDiff();
		pendStateDiff(const pendState& p, const double accel[3], const double dt);
	};

	pendState::pendState(){
		theta[0] = 3.0;
		theta[1] = 3.5;
		theta[2] = 1.5;
		dtheta[0] = 1.0;
		dtheta[1] = 1.5;
		dtheta[2] = -5.0;
	}
	void pendState::addDiff(const pendState p, const pendStateDiff pd, double r){
		for(int i = 0; i < 3; i++){
			theta[i] = p.theta[i] + r*pd.diff[i];
			dtheta[i] = p.dtheta[i] + r*pd.diff[i+3];
		}
	}
	void pendState::adjust(){
		for(int i = 0; i < 3; i++){
			if(theta[i] < 0.0) theta[i] += 2*PI;
			else if(theta[i] > 2*PI) theta[i] -= 2*PI;
		}
	}
	pendStateDiff::pendStateDiff(){};
	pendStateDiff::pendStateDiff(const pendState& p, const double accel[3], const double dt){
		for(int i = 0; i < 3; i++){
			diff[i] = dt*p.dtheta[i];
			diff[i+3] = dt*accel[i];
		}
	}

	class pendClass{
		pendState pend;
		double length[3];
		double mass[3];
		int integrateMethod;
	public:
		pendClass(){
			length[0] = 0.4;
			length[1] = 0.4;
			length[2] = 0.2;
			mass[0] = 2.0;
			mass[1] = 2.0;
			mass[2] = 1.0;
			integrateMethod = 1;
		}

		void initialize(){
			pend = pendState();
		}

		void setIntegrate(int _integrateMethod){
			integrateMethod = _integrateMethod;
		}

		std::string getIntegrateName(){
			switch(integrateMethod){
			case 1:
				return "Euler Method";
			case 2:
				return "Heun's Method";
			case 3:
				return "Runge-Kutta Method";
			default:
				return "Unknown";
			}
		}

		//return vertices of pendulums (contains 4 pairs)
		std::vector<std::pair<double, double> > vertices(){
			std::vector<std::pair<double, double> > ans;
			std::pair<double, double> tmpV(0.0, 0.0);
			ans.push_back(tmpV);
			for(int i = 0; i < 3; i++){
				tmpV.first += length[i]*sin(pend.theta[i]);
				tmpV.second -= length[i]*cos(pend.theta[i]);
				ans.push_back(tmpV);
			}
			return ans;
		}

		double T(){ return calcT(pend);}
		double U(){ return calcU(pend);}
		double energy(){ return calcEnergy(pend);}

		void move(const double dt){
			//integrate
			switch(integrateMethod){
			case 1:
				integrate_euler(dt);
				break;
			case 2:
				integrate_heun(dt);
				break;
			case 3:
				integrate_runge_kutta(dt);
				break;
			}
			//0 < theta < 2*PI
			pend.adjust();
		}

	private:
		void integrate_euler(const double dt){
			double accel[3];
			calcAccel(pend, accel);
			pendStateDiff k1(pend, accel, dt);

			pend.addDiff(pend, k1, 1.0);
		}

		void integrate_heun(const double dt){
			double accel[3];
			pendState p1;
			calcAccel(pend, accel);
			pendStateDiff k1(pend, accel, dt);

			p1.addDiff(pend, k1, 1.0/2.0);
			calcAccel(p1, accel);
			pendStateDiff k2(p1, accel, dt);

			pendStateDiff k;
			for(int i = 0; i < 6; i++){
				k.diff[i] = 1.0/2.0*(k1.diff[i]+k2.diff[i]);
			}
			pend.addDiff(pend, k, 1.0);
		}

		void integrate_runge_kutta(const double dt){
			double accel[3];
			pendState p1, p2, p3;
			calcAccel(pend, accel);
			pendStateDiff k1(pend, accel, dt);

			p1.addDiff(pend, k1, 1.0/2.0);
			calcAccel(p1, accel);
			pendStateDiff k2(p1, accel, dt);

			p2.addDiff(pend, k2, 1.0/2.0);
			calcAccel(p2, accel);
			pendStateDiff k3(p2, accel, dt);

			p3.addDiff(pend, k3, 1.0);
			calcAccel(p3, accel);
			pendStateDiff k4(p3, accel, dt);

			pendStateDiff k;
			for(int i = 0; i < 6; i++){
				k.diff[i] = 1.0/6.0*(k1.diff[i]+2.0*k2.diff[i]+2.0*k3.diff[i]+k4.diff[i]);
			}
			pend.addDiff(pend, k, 1.0);
		}

		//calculation of generalized acceleration
		void calcAccel(const pendState& p, double accel[3]){
			//set matrices
			double accelMat[3][3];
			accelMat[0][0] = 1.0/3.0*mass[0]+mass[1]+mass[2];
			accelMat[1][1] = 1.0/3.0*mass[1]+mass[2];
			accelMat[2][2] = 1.0/3.0*mass[2];
			accelMat[0][1] = accelMat[1][0] = (1.0/2.0*mass[1]+mass[2])*cos(p.theta[0] - p.theta[1]);
			accelMat[0][2] = accelMat[2][0] = 1.0/2.0*mass[2]*cos(p.theta[0] - p.theta[2]);
			accelMat[1][2] = accelMat[2][1] = 1.0/2.0*mass[2]*cos(p.theta[1] - p.theta[2]);

			double forceMat1[3][3];
			forceMat1[0][0] = forceMat1[1][1] = forceMat1[2][2] = 0.0;
			forceMat1[0][1] = -(1.0/2.0*mass[1]+mass[2])*sin(p.theta[0] - p.theta[1]);
			forceMat1[1][0] = -forceMat1[0][1];
			forceMat1[0][2] = -1.0/2.0*mass[2]*sin(p.theta[0] - p.theta[2]);
			forceMat1[2][0] = -forceMat1[0][2];
			forceMat1[1][2] = -1.0/2.0*mass[2]*sin(p.theta[1] - p.theta[2]);
			forceMat1[2][1] = -forceMat1[1][2];

			//set right value
			double forceVec[3];
			forceVec[0] = -(1.0/2.0*mass[0]+mass[1]+mass[2])*g*sin(p.theta[0]);
			forceVec[1] = -(1.0/2.0*mass[1]+mass[2])*g*sin(p.theta[1]);
			forceVec[2] = -(1.0/2.0*mass[2])*g*sin(p.theta[2]);
			for(int i = 0; i < 3; i++){
				for(int j = 0; j < 3; j++){
					forceVec[i] += forceMat1[i][j] * length[j]*p.dtheta[j]*p.dtheta[j];
				}
			}

			//Gauss-Jordan  elimination
			for(int i = 0; i < 3; i++){
				double tmpElem = accelMat[i][i];
				forceVec[i] /= tmpElem;
				for(int k = i; k < 3; k++){
					accelMat[i][k] /= tmpElem;
				}
				for(int j = 0; j < 3; j++){
					if(i==j) continue;
					tmpElem = accelMat[j][i];
					forceVec[j] -= tmpElem*forceVec[i];
					for(int k = i; k < 3; k++){
						accelMat[j][k] -= tmpElem*accelMat[i][k];
					}
				}
			}

			for(int i = 0; i < 3; i++){
				accel[i] = forceVec[i] / length[i];
			}
		}

		//Kinetic Energy
		double calcT(const pendState& p){
			return (1.0/6.0*mass[0]+1.0/2.0*mass[1]+1.0/2.0*mass[2])*length[0]*length[0]*p.dtheta[0]*p.dtheta[0]
			     + (1.0/6.0*mass[1]+1.0/2.0*mass[2])                *length[1]*length[1]*p.dtheta[1]*p.dtheta[1]
			     + (1.0/6.0*mass[2])                                *length[2]*length[2]*p.dtheta[2]*p.dtheta[2]
			     + (1.0/2.0*mass[1]+mass[2])                        *length[0]*length[1]*p.dtheta[0]*p.dtheta[1]*cos(p.theta[0]-p.theta[1])
			     + (1.0/2.0*mass[2])                                *length[0]*length[2]*p.dtheta[0]*p.dtheta[2]*cos(p.theta[0]-p.theta[2])
			     + (1.0/2.0*mass[2])                                *length[1]*length[2]*p.dtheta[1]*p.dtheta[2]*cos(p.theta[1]-p.theta[2]);
		}

		//Potential Energy
		double calcU(const pendState& p){
			return -g*(
				  (1.0/2.0*mass[0]+mass[1]+mass[2])*length[0]*(-1.0+cos(p.theta[0]))
				+ (1.0/2.0*mass[1]+mass[2])        *length[1]*(-1.0+cos(p.theta[1]))
				+ (1.0/2.0*mass[2])                *length[2]*(-1.0+cos(p.theta[2]))
				);
		}

		double calcEnergy(const pendState& p){
			return calcT(p)+calcU(p);
		}
	};
}

#endif
