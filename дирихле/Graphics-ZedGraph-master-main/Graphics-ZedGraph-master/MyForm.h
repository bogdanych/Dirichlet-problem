#pragma once
#include "../MBR/MBR/MBR.h"
#include <sstream>

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
			if (solver) {
				delete solver;
				solver = nullptr;
			}

			if (components)
			{
				delete components;
			}
		}

	private:
		System::Windows::Forms::TabControl^ tabControl1;
		System::Windows::Forms::TabPage^ tabPageSolution;
		System::Windows::Forms::TabPage^ tabPageExact;
		System::Windows::Forms::TabPage^ tabPageError;
		System::Windows::Forms::DataGridView^ dataGridViewSolution;
		System::Windows::Forms::DataGridView^ dataGridViewExact;
		System::Windows::Forms::DataGridView^ dataGridViewError;
		System::Windows::Forms::Button^ button1;
		System::Windows::Forms::Label^ label1;
		System::Windows::Forms::Label^ label2;
		System::Windows::Forms::Label^ label3;
		System::Windows::Forms::Label^ label4;
		System::Windows::Forms::Label^ label5;
		System::Windows::Forms::Label^ label6;
		System::Windows::Forms::TextBox^ txtN;
		System::Windows::Forms::TextBox^ txtM;
		System::Windows::Forms::TextBox^ txtReference;
		System::ComponentModel::IContainer^ components;

		SORSolver* solver;

		void InitializeComponent(void)
		{
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPageSolution = (gcnew System::Windows::Forms::TabPage());
			this->tabPageExact = (gcnew System::Windows::Forms::TabPage());
			this->tabPageError = (gcnew System::Windows::Forms::TabPage());
			this->dataGridViewSolution = (gcnew System::Windows::Forms::DataGridView());
			this->dataGridViewExact = (gcnew System::Windows::Forms::DataGridView());
			this->dataGridViewError = (gcnew System::Windows::Forms::DataGridView());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->txtN = (gcnew System::Windows::Forms::TextBox());
			this->txtM = (gcnew System::Windows::Forms::TextBox());
			this->txtReference = (gcnew System::Windows::Forms::TextBox());

			this->tabControl1->SuspendLayout();
			this->tabPageSolution->SuspendLayout();
			this->tabPageExact->SuspendLayout();
			this->tabPageError->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridViewSolution))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridViewExact))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridViewError))->BeginInit();
			this->SuspendLayout();

			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPageSolution);
			this->tabControl1->Controls->Add(this->tabPageExact);
			this->tabControl1->Controls->Add(this->tabPageError);
			this->tabControl1->Location = System::Drawing::Point(12, 80);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(800, 430);
			this->tabControl1->TabIndex = 0;

			// 
			// tabPageSolution
			// 
			this->tabPageSolution->Controls->Add(this->dataGridViewSolution);
			this->tabPageSolution->Location = System::Drawing::Point(4, 22);
			this->tabPageSolution->Name = L"tabPageSolution";
			this->tabPageSolution->Padding = System::Windows::Forms::Padding(3);
			this->tabPageSolution->Size = System::Drawing::Size(792, 404);
			this->tabPageSolution->TabIndex = 0;
			this->tabPageSolution->Text = L"Численное решение";
			this->tabPageSolution->UseVisualStyleBackColor = true;

			// 
			// tabPageExact
			// 
			this->tabPageExact->Controls->Add(this->dataGridViewExact);
			this->tabPageExact->Location = System::Drawing::Point(4, 22);
			this->tabPageExact->Name = L"tabPageExact";
			this->tabPageExact->Padding = System::Windows::Forms::Padding(3);
			this->tabPageExact->Size = System::Drawing::Size(792, 404);
			this->tabPageExact->TabIndex = 1;
			this->tabPageExact->Text = L"Точное решение";
			this->tabPageExact->UseVisualStyleBackColor = true;

			// 
			// tabPageError
			// 
			this->tabPageError->Controls->Add(this->dataGridViewError);
			this->tabPageError->Location = System::Drawing::Point(4, 22);
			this->tabPageError->Name = L"tabPageError";
			this->tabPageError->Padding = System::Windows::Forms::Padding(3);
			this->tabPageError->Size = System::Drawing::Size(792, 404);
			this->tabPageError->TabIndex = 2;
			this->tabPageError->Text = L"Погрешность";
			this->tabPageError->UseVisualStyleBackColor = true;

			// 
			// dataGridViewSolution
			// 
			this->dataGridViewSolution->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridViewSolution->Dock = System::Windows::Forms::DockStyle::Fill;
			this->dataGridViewSolution->Name = L"dataGridViewSolution";
			this->dataGridViewSolution->RowHeadersVisible = false;
			this->dataGridViewSolution->TabIndex = 0;

			// 
			// dataGridViewExact
			// 
			this->dataGridViewExact->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridViewExact->Dock = System::Windows::Forms::DockStyle::Fill;
			this->dataGridViewExact->Name = L"dataGridViewExact";
			this->dataGridViewExact->RowHeadersVisible = false;
			this->dataGridViewExact->TabIndex = 0;

			// 
			// dataGridViewError
			// 
			this->dataGridViewError->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridViewError->Dock = System::Windows::Forms::DockStyle::Fill;
			this->dataGridViewError->Name = L"dataGridViewError";
			this->dataGridViewError->RowHeadersVisible = false;
			this->dataGridViewError->TabIndex = 0;

			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(220, 520);
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
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9));
			this->label2->Location = System::Drawing::Point(12, 560);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(200, 15);
			this->label2->TabIndex = 3;

			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9));
			this->label3->Location = System::Drawing::Point(12, 585);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(200, 15);
			this->label3->TabIndex = 4;
			this->label3->Text = L"Серые ячейки - граничные условия";

			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Italic));
			this->label4->Location = System::Drawing::Point(12, 610);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(200, 15);
			this->label4->TabIndex = 5;
			this->label4->Text = L"Решение уравнения Пуассона методом SOR";

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
			// txtReference
			// 
			this->txtReference = (gcnew System::Windows::Forms::TextBox());
			this->txtReference->Location = System::Drawing::Point(12, 640);
			this->txtReference->Multiline = true;
			this->txtReference->Name = L"txtReference";
			this->txtReference->ReadOnly = true;
			this->txtReference->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->txtReference->Size = System::Drawing::Size(800, 200);
			this->txtReference->TabIndex = 10;
			this->txtReference->Font = (gcnew System::Drawing::Font(L"Consolas", 9, System::Drawing::FontStyle::Regular));

			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(830, 860);
			this->Controls->Add(this->txtReference);
			this->Controls->Add(this->txtM);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->txtN);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->tabControl1);
			this->Name = L"MyForm";
			this->Text = L"Решение уравнения методом SOR";

			this->tabControl1->ResumeLayout(false);
			this->tabPageSolution->ResumeLayout(false);
			this->tabPageExact->ResumeLayout(false);
			this->tabPageError->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridViewSolution))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridViewExact))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridViewError))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();
		}

	private:
		void DisplaySolutionInGrid(DataGridView^ grid, bool showExact, bool showError) {
			grid->Rows->Clear();
			grid->Columns->Clear();

			int N = solver->getN();
			int M = solver->getM();
			double h = solver->getH();
			double k = solver->getK();

			// Создаем колонки
			grid->Columns->Add("X\\Y", "X\\Y");
			grid->Columns["X\\Y"]->Width = 60;

			for (int j = 0; j <= M; j++) {
				double y = -1.0 + j * k;
				String^ colName = String::Format("y={0:F3}", y);
				grid->Columns->Add(colName, colName);
				grid->Columns[colName]->Width = 70;
			}

			// Добавляем строки
			for (int i = 0; i <= N; i++) {
				double x = -1.0 + i * h;

				array<Object^>^ rowData = gcnew array<Object^>(M + 2);
				rowData[0] = String::Format("{0:F3}", x);

				for (int j = 0; j <= M; j++) {
					// Проверяем, принадлежит ли узел L-образной области
					bool inDomain = solver->is_in_domain(i, j);


					if (!inDomain) {
						rowData[j + 1] = "";  // Пустая ячейка для вырезанной области
					}
					else {
						double value;

						if (showExact) {
							value = solver->getExactSolution(i, j);
						}
						else if (showError) {
							value = solver->getError(i, j);
						}
						else {
							value = solver->getSolution(i, j);
						}

						rowData[j + 1] = String::Format("{0:F6}", value);
					}
				}

				int rowIndex = grid->Rows->Add(rowData);

				// Подсвечиваем вырезанную область серым
				for (int j = 0; j <= M; j++) {
					if (!solver->is_in_domain(i,j)) {
						grid->Rows[rowIndex]->Cells[j + 1]->Style->BackColor = Drawing::Color::LightGray;
					}
				}
			}

			grid->AutoResizeColumns();
		}

		void DisplayAllTables() {
			DisplaySolutionInGrid(dataGridViewSolution, false, false);
			DisplaySolutionInGrid(dataGridViewExact, true, false);
			DisplaySolutionInGrid(dataGridViewError, false, true);
		}

		void DisplayReference() {
			if (solver != nullptr) {
				String^ reference = "";

				reference += "Справка для тестовой задачи\r\n\r\n";

				reference += "Для решения тестовой задачи использована сетка-основа\r\n";
				reference += "с числом разбиений по x n = " + Convert::ToString(solver->getN()) +
					" и числом разбиений по y m = " + Convert::ToString(solver->getM()) + ",\r\n\r\n";

				reference += "метод: Метод верхней релаксации (МВР),\r\n";
				reference += "параметры: w = " + String::Format("{0:F6}", solver->getW()) +
					" (оптимальное значение для сетки: w_opt = 2/(1+sin(pi/n)))\r\n\r\n";

				reference += "критерии остановки: по точности eps_met = " + String::Format("{0:E6}", solver->getEps()) +
					" и по числу итераций N_max = " + Convert::ToString(solver->getNmax()) + "\r\n\r\n";

				reference += "На решение схемы (СЛАУ) затрачено итераций N = " +
					Convert::ToString(solver->getTotalIterations()) +
					" и достигнута точность итерационного метода eps(N) = " +
					String::Format("{0:E6}", solver->getFinalResidual()) + "\r\n\r\n";

				reference += "Схема (СЛАУ) решена с невязкой ||R(N)||_max = " +
					String::Format("{0:E6}", solver->getResidualNormMax()) + " (норма max)\r\n";
				reference += "Схема (СЛАУ) решена с невязкой ||R(N)||_2 = " +
					String::Format("{0:E6}", solver->getResidualNormEuclid()) + " (евклидова норма)\r\n\r\n";

				reference += "для невязки СЛАУ использована норма max и евклидова норма\r\n\r\n";

				reference += "Тестовая задача должна быть решена с погрешностью не более eps = 0.5*10^(-6);\r\n";
				reference += "задача решена с погрешностью eps1 = " +
					String::Format("{0:E6}", solver->getFinalError()) + "\r\n\r\n";

				reference += "Максимальное отклонение точного и численного решений\r\n";
				reference += "наблюдается в узле:\r\n";
				reference += "x = " + String::Format("{0:F6}", solver->getErrorNodeX()) + "\r\n";
				reference += "y = " + String::Format("{0:F6}", solver->getErrorNodeY()) + "\r\n";
				reference += "индекс узла: i = " + Convert::ToString(solver->getErrorNodeI()) +
					", j = " + Convert::ToString(solver->getErrorNodeJ()) + "\r\n\r\n";

				reference += "В качестве начального приближения использовано: нулевое начальное приближение\r\n\r\n";

				reference += "Время вычислений: " + Convert::ToString(solver->getSolveTime()) + " мс\r\n";

				txtReference->Text = reference;
			}
		}

		void SolveAndDisplay() {
			solver->init();
			solver->solve();
			DisplayAllTables();
			DisplayReference();
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