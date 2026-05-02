#pragma once
#include "MBR.h"

namespace Graph {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();

			txtN->Text = "10";
			txtM->Text = "10";

			int N = Convert::ToInt32(txtN->Text);
			int M = Convert::ToInt32(txtM->Text);
			solver = new SORSolver(N, M);
			SolveAndDisplay();
		}

	protected:
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
			delete solver;
		}

	private:
		System::Windows::Forms::DataGridView^ dataGridView1;
		System::Windows::Forms::Button^ button1;
		System::Windows::Forms::Label^ label1;
		System::Windows::Forms::Label^ label2;
		System::Windows::Forms::Label^ label3;
		System::Windows::Forms::Label^ label4;
		System::Windows::Forms::Label^ label5;
		System::Windows::Forms::Label^ label6;
		System::Windows::Forms::TextBox^ txtN;
		System::Windows::Forms::TextBox^ txtM;
		System::ComponentModel::IContainer^ components;

		SORSolver* solver;

		void InitializeComponent(void)
		{
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->txtN = (gcnew System::Windows::Forms::TextBox());
			this->txtM = (gcnew System::Windows::Forms::TextBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->BeginInit();
			this->SuspendLayout();
			// 
			// dataGridView1
			// 
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Location = System::Drawing::Point(12, 80);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->RowHeadersVisible = false;
			this->dataGridView1->Size = System::Drawing::Size(800, 400);
			this->dataGridView1->TabIndex = 0;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(220, 490);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(150, 30);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Решить";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold));
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(300, 20);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Параметры сетки:";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10));
			this->label5->Location = System::Drawing::Point(12, 40);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(25, 17);
			this->label5->TabIndex = 6;
			this->label5->Text = L"N =";
			// 
			// txtN
			// 
			this->txtN->Location = System::Drawing::Point(40, 38);
			this->txtN->Name = L"txtN";
			this->txtN->Size = System::Drawing::Size(60, 20);
			this->txtN->TabIndex = 7;
			this->txtN->Text = L"20";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10));
			this->label6->Location = System::Drawing::Point(120, 40);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(25, 17);
			this->label6->TabIndex = 8;
			this->label6->Text = L"M =";
			// 
			// txtM
			// 
			this->txtM->Location = System::Drawing::Point(148, 38);
			this->txtM->Name = L"txtM";
			this->txtM->Size = System::Drawing::Size(60, 20);
			this->txtM->TabIndex = 9;
			this->txtM->Text = L"20";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9));
			this->label2->Location = System::Drawing::Point(12, 530);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(200, 15);
			this->label2->TabIndex = 3;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9));
			this->label3->Location = System::Drawing::Point(12, 555);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(200, 15);
			this->label3->TabIndex = 4;
			this->label3->Text = L"Серые ячейки - граничные условия";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Italic));
			this->label4->Location = System::Drawing::Point(12, 580);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(200, 15);
			this->label4->TabIndex = 5;
			this->label4->Text = L"Решение уравнения Пуассона методом SOR";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(830, 620);
			this->Controls->Add(this->txtM);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->txtN);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->dataGridView1);
			this->Name = L"MyForm";
			this->Text = L"Решение уравнения методом SOR";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();
		}

	private:
		void DisplaySolution() {
			dataGridView1->Rows->Clear();
			dataGridView1->Columns->Clear();

			int N = solver->getN();
			int M = solver->getM();
			double h = solver->getH();
			double k = solver->getK();

			dataGridView1->Columns->Add("X\\Y", "X\\Y");
			dataGridView1->Columns["X\\Y"]->Width = 60;

			for (int j = 0; j <= M; j++) {
				double y = -1.0 + j * k;
				String^ colName = String::Format("y={0:F3}", y);
				dataGridView1->Columns->Add(colName, colName);
				dataGridView1->Columns[colName]->Width = 70;
			}

			for (int i = 0; i <= N; i++) {
				double x = -1.0 + i * h;
				DataGridViewRow^ row = gcnew DataGridViewRow();
				row->CreateCells(dataGridView1);

				row->Cells[0]->Value = String::Format("{0:F3}", x);

				for (int j = 0; j <= M; j++) {
					double solution = solver->getSolution(i, j);
					row->Cells[j + 1]->Value = String::Format("{0:F6}", solution);

					if (i == 0 || i == N || j == 0 || j == M) {
						row->Cells[j + 1]->Style->BackColor = Drawing::Color::LightGray;
					}
				}

				dataGridView1->Rows->Add(row);
			}


			label1->Text = String::Format("Решение для сетки: N = {0}, M = {1}  |  h = {2:F4}  |  k = {3:F4}",
				N, M, h, k);
			label2->Text = String::Format("Максимальная погрешность: {0:E6}", solver->getError());

			dataGridView1->AutoResizeColumns();
		}
	

		void SolveAndDisplay() {
			solver->init();
			solver->solve();
			DisplaySolution();
		}

	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
		int N = Convert::ToInt32(txtN->Text);
		int M = Convert::ToInt32(txtM->Text);


		delete solver;
		solver = new SORSolver(N, M);
		SolveAndDisplay();

		MessageBox::Show(String::Format("Решение обновлено для сетки {0}x{1}!", N, M), "Готово");
		

	}
	};
}