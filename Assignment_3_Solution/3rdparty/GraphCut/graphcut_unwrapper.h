#ifndef _GRAPHCUT_UNWRAPPER_H_
#define _GRAPHCUT_UNWRAPPER_H_

#include <iostream>
#include "graph.h"

using namespace std;

namespace MRF {

const double PI = 3.1415926535;

	struct Pixel
	{
		// wrapped phase
		int m_wrapped_phase;
		// wrapped k
		int m_k;
		// source weight
		double m_S_weight;
		// sink weight
		double m_T_weight;
		// graph node
		Graph::node_id m_graph_node = nullptr;
	};

	class CGraphcutunwrapper
	{

	private:
		// image size
		int m_rows;
		int m_cols;
		// initialize 2D Pixel struct array
		Pixel** m_pixels = nullptr;

	public:
		// constructor
		CGraphcutunwrapper(int rows, int cols) :m_rows(rows), m_cols(cols)
		{
			m_pixels = new Pixel*[m_rows];
			for (int i = 0; i < m_rows; i++)
				m_pixels[i] = new Pixel[m_cols];

		};

		// read wrapped phase
		void _input_wrapped_phase(const double* _image)
		{
			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols; j++)
					m_pixels[i][j].m_wrapped_phase = _image[i * m_cols + j];

		}

		// get unwrapped phase
		void _output_wrapped_count(double* _image)
		{
			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols; j++)
					_image[i * m_cols + j] = m_pixels[i][j].m_k;

		}

		// get wrapped count
		void _output_unwrapped_phase(double* _image)
		{
			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols; j++)
					_image[i * m_cols + j] = m_pixels[i][j].m_wrapped_phase + 2 * PI * m_pixels[i][j].m_k;

		}

		// total edge energy
		double _energy() 
		{

			// change memory release mode
			static int times = 0;
			double energy = 0;

			// ADD horizontal edge energy
			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols - 1; j++)
				{
					Pixel* P = &m_pixels[i][j + 0];
					Pixel* Q = &m_pixels[i][j + 1];

					double phaseP = P->m_wrapped_phase + P->m_k * 2 * PI;
					double phaseQ = Q->m_wrapped_phase + Q->m_k * 2 * PI;

					double phaseD = phaseP - phaseQ;
					energy += phaseD * phaseD;

				}

			// ADD vertical edge energy
			for (int i = 0; i < m_rows - 1; i++)
				for (int j = 0; j < m_cols; j++)
				{
					Pixel* P = &m_pixels[i + 0][j];
					Pixel* Q = &m_pixels[i + 1][j];

					double phaseP = P->m_wrapped_phase + P->m_k * 2 * PI;
					double phaseQ = Q->m_wrapped_phase + Q->m_k * 2 * PI;

					double phaseD = phaseP - phaseQ;
					energy += phaseD * phaseD;

				}

			// ADD cross edge energy: Arrow lower right
			int neighbors1[1][2] = { {+1, -1} };

			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols; j++)
				{
					Pixel* P = &m_pixels[i + 0][j];

					double phaseP = P->m_wrapped_phase + P->m_k * 2 * PI;

					//double phaseQ = 0;

					for (int k = 0; k < 1; k++)
					{
						int x = i + neighbors1[k][0];
						int y = j + neighbors1[k][1];

						if (!_in_range(x, y)) continue;

						// S stands for surrounding pixel
						Pixel* Q = &m_pixels[x][y];

						double phaseQ = Q->m_wrapped_phase + Q->m_k * 2 * PI;

						double phaseD = phaseP - phaseQ;
						energy += phaseD * phaseD;
					}

					
				}

				// ADD cross edge energy: Arrow upper right 
				int neighbors2[1][2] = { {+1, +1} };

				for (int i = 0; i < m_rows; i++)
					for (int j = 0; j < m_cols; j++)
					{
						Pixel* P = &m_pixels[i + 0][j];

						double phaseP = P->m_wrapped_phase + P->m_k * 2 * PI;

						//double phaseQ = 0;

						for (int k = 0; k < 1; k++)
						{
							int x = i + neighbors2[k][0];
							int y = j + neighbors2[k][1];

							if (!_in_range(x, y)) continue;

							// S stands for surrounding pixel
							Pixel* Q = &m_pixels[x][y];

							double phaseQ = Q->m_wrapped_phase + Q->m_k * 2 * PI;

							double phaseD = phaseP - phaseQ;
							energy += phaseD * phaseD;

						}
						
					}

			// print iteration and corresponding total edge energy
			cout << "\n Iteration " << times++ << ": Total energy is:  " << energy << endl;

			return energy;
		}

		bool _in_range(int x, int y)
		{
			return (x >= 0 && x < m_rows&& y >= 0 && y < m_cols);
		}

		// total linked weight for at pixel P
		void _calculate_linked_weight()
		{
			
			int neighbors[4][2] = { {-1,0},
									{+1,0},
									{0,-1},
									{0,+1} };

			int corners[4][2] = { {-1,+1},
								  {+1,+1},
								  {-1,-1},
								  {+1,-1} };


			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols; j++)
				{
					Pixel* P = &m_pixels[i][j];

					double LW = 0;

					// P stands for current pixel
					double P_unwrapped_phase = P->m_wrapped_phase + 2 * PI * P->m_k;



					// 4 adjacent tile
					for (int k = 0; k < 4; k++)
					{

						int x = i + neighbors[k][0];
						int y = j + neighbors[k][1];

						if (!_in_range(x, y)) continue;

						// S stands for surrounding pixel
						Pixel* S = &m_pixels[x][y];

						double S_unwrapped_phase = S->m_wrapped_phase + 2 * PI * S->m_k;

						LW += 4 * PI * (P_unwrapped_phase - S_unwrapped_phase) ;
					}
					

					// 4 cross tiles
					for (int k = 0; k < 4; k++)
					{

						int x = i + corners[k][0];
						int y = j + corners[k][1];

						if (!_in_range(x, y)) continue;

						// S stands for surrounding pixel
						Pixel* S = &m_pixels[x][y];

						double S_unwrapped_phase = S->m_wrapped_phase + 2 * PI * S->m_k;

						LW += 4 * PI * (P_unwrapped_phase - S_unwrapped_phase);
					}

				P->m_S_weight = 0;
				P->m_T_weight = LW;
				// cout << LW << " ";
						
				}

			double c = 1e+10;

			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols; j++) 
				{
					Pixel* P = &m_pixels[i][j];

					c = (c < P->m_S_weight) ? c : P->m_S_weight;
					c = (c < P->m_T_weight) ? c : P->m_T_weight;

				}

			if (c > 0) return;

			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols; j++)
				{
					Pixel* P = &m_pixels[i][j];

					P->m_S_weight += (1 - c);
					P->m_T_weight += (1 - c);
					//cout << P->m_T_weight << " " << endl;
				}
		}
		
		void _calculate_maxflow_mincut() 
		{

			Graph *g = new Graph();

			for (int i = 0; i < m_rows; i++) 
				for (int j = 0 ; j < m_cols; j++) 
				{
					Pixel* P = &m_pixels[i][j];
					P->m_graph_node = g->add_node();
				}

			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols; j++)
				{
					Pixel* P = &m_pixels[i][j];

					g->set_tweights(P->m_graph_node, P->m_S_weight, P->m_T_weight);
				}

			const double edge_weight = 4.0 * PI * PI;

			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols - 1; j++)
				{
					Pixel* P = &m_pixels[i][j + 0];
					Pixel* Q = &m_pixels[i][j + 1];

					g->add_edge(P->m_graph_node, Q->m_graph_node, edge_weight, edge_weight);
				}


			for (int i = 0; i < m_rows - 1; i++)
				for (int j = 0; j < m_cols; j++)
				{
					Pixel* P = &m_pixels[i + 0][j];
					Pixel* Q = &m_pixels[i + 1][j];

					g->add_edge(P->m_graph_node, Q->m_graph_node, edge_weight, edge_weight);
				}

			
			int cross1[1][2] = { {+1, -1} };
								     			 
			// Add cross edges
			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols; j++)
				{
					Pixel* P = &m_pixels[i + 0][j];

					for (int k = 0; k < 1; k++)
					{
						int x = i + cross1[k][0];
						int y = j + cross1[k][1];

						if (!_in_range(x, y)) continue;

						// S stands for surrounding pixel
						Pixel* S = &m_pixels[x][y];

						g->add_edge(P->m_graph_node, S->m_graph_node, edge_weight, edge_weight);

					}
				}
					
				int cross2[1][2] = { {+1, +1} };
										 
				// Add cross edges
				for (int i = 0; i < m_rows; i++)
					for (int j = 0; j < m_cols; j++)
					{
						Pixel* P = &m_pixels[i + 0][j];

						for (int k = 0; k < 1; k++)
						{
							int x = i + cross2[k][0];
							int y = j + cross2[k][1];

							if (!_in_range(x, y)) continue;

							// S stands for surrounding pixel
							Pixel* S = &m_pixels[x][y];

							g->add_edge(P->m_graph_node, S->m_graph_node, edge_weight, edge_weight);

						}
					}

			Graph::flowtype flow = g->maxflow();
			// printf("Flow = %d\n", flow);

			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols; j++)
				{
					Pixel* P = &m_pixels[i][j];

					if (g->what_segment(P->m_graph_node) == Graph::SOURCE)
						P->m_k += 1;
					else
						P->m_k += 0;
				}

			delete g;
		}

		void _unwrapping() 
		{ 
			for (int i = 0; i < m_rows; i++)
				for (int j = 0; j < m_cols; j++)
				{
					Pixel* P = &m_pixels[i][j];
					P->m_k = 0;
				}

			double old_energy = _energy();
		
			while (true) 
			{

				_calculate_linked_weight();
				_calculate_maxflow_mincut();

				double new_energy = _energy();

				if (new_energy >= old_energy) break;

					old_energy = new_energy;
			}
		}

		// destructor 
		~CGraphcutunwrapper()
		{
			for (int i = 0; i < m_rows; i++)
				delete[]m_pixels[i];
			delete m_pixels;
		}
		// array of pixels 
		Pixel** pixels() {
			return m_pixels;
		}
	};			
}

#endif