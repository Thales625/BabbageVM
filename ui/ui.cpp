#include "ui.hpp"

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

int UI::setup(Memory<word_t>* mem_ptr) {
	this->mem_ptr = mem_ptr;

	// SDL init...
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    window = SDL_CreateWindow("BabbageVM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

	return 0;
}

void UI::run() {
	std::string dropped_file; // for dropping the input file
	std::string file_content;

	// Main loop
	while(!done){
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
		while(SDL_PollEvent(&event)){
			ImGui_ImplSDL2_ProcessEvent(&event);
			if(event.type == SDL_QUIT)
				done = true;
			if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)
				done = true;

			// drag & drop do input.txt
			if(event.type == SDL_DROPFILE){
				char* dropped_path = event.drop.file;
				dropped_file = dropped_path;
				std::cout << "Arquivo solto: " << dropped_path << std::endl;
				file_ready = true;
				SDL_free(dropped_path); // sempre liberar!

				std::ifstream file(dropped_file);
				if(file){
					std::stringstream buffer;
					buffer << file.rdbuf();
					file_content = buffer.str();
				}else{
					file_content = "Failed to open file";
				}
			}
		}
		if(SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED){
			SDL_Delay(10);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// Controls window -> run, step through the generated code and clear
		{
			// static float f = 0.0f;

			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(250, 200), ImGuiCond_Always);
			ImGui::Begin("CONTROLS", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); // Create window

			// TODO: clicar nos botões e fazer coisas
			// if(componente) -> evento, faz coisas dentro do if?
			if(ImGui::Button("(R)un") || ImGui::IsKeyPressed(ImGuiKey_R)){
				std::cout << "run ";
				// Faz tudo aqui
			}

			ImGui::SameLine();
			if(ImGui::Button("(S)tep") || ImGui::IsKeyPressed(ImGuiKey_S)){
				std::cout << "step ";
				// nao precisa implementar
			}

			ImGui::SameLine();
			if(ImGui::Button("(C)lear") || ImGui::IsKeyPressed(ImGuiKey_C)){
				std::cout << "clear ";
				// clear symtable talvez?
			}

			// TODO: etapas separadas aqui
			if(ImGui::Button("Pre-process") || ImGui::IsKeyPressed(ImGuiKey_R)){
				std::cout << "pre-processing...\n";
				// ---------
				//std::string filename = argv[1];
				//std::vector<std::string> lines = Macro::ProcessFromFile(filename);
				//save_file(lines, "bin/macro_out.bbg");
				// ---------
				std::cout << "pre-process done\n";
			}
			if(ImGui::Button("Assemble") || ImGui::IsKeyPressed(ImGuiKey_R)){
				std::cout << "assembling...\n";
				// ---------
				//Assembler assembler;
				//assembler.assemble("bin/macro_out.bbg");
				// ---------
				std::cout << "assemble done\n";
			}
			if(ImGui::Button("Link & Load") || ImGui::IsKeyPressed(ImGuiKey_R)){
				std::cout << "linking...\n";
				// ---------
				//Linker linker;
				//linker.link({"bin/assembler_out.OBJ"}, "bin/linker_out.hpx");
				// ---------
				std::cout << "link done\n";
				std::cout << "loading...\n";
				// ---------
				//Loader loader;
				//loader.load("bin/linker_out.HPX");
				// ---------
				std::cout << "loaded\n";

			}

			ImGui::Text("avg %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
			ImGui::Text("press Q to quit");
			ImGui::End();
		}

		// Memory -> table with memory addrs and contents
		{
			static int offset = 0;
			ImGui::SetNextWindowPos(ImVec2(250, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(1030, 460), ImGuiCond_Always);
			ImGui::Begin("MEMORY", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			// ImGui::SliderInt("OFFSET", &offset, 0, sizeof(memory) - bytes_row * rows);
			ImGui::SameLine();
			ImGui::ProgressBar(0.5f, ImVec2(200, 20), "USED"); //TODO: occupied mem here

			if(ImGui::BeginTable("MemoryTable", bytes_row + 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)){
				ImGui::TableSetupColumn("Addr");
				for (int i = 0; i < bytes_row; i++){
					char col_name[8];
					sprintf(col_name, "+%X", i);
					ImGui::TableSetupColumn(col_name);
				}

				ImGui::TableSetupColumn("ASCII");
				ImGui::TableHeadersRow();

				for(int row = 0; row < rows; row++){
					int addr = offset + row * bytes_row;
					if(addr >= this->mem_ptr->get_size()) break;

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%04X", addr);

					char ascii[32];
					ascii[bytes_row] = '\0';

					for (int col = 0; col < bytes_row; ++col) {
						int index = addr + col;
						ImGui::TableSetColumnIndex(col + 1);

						if (index < this->mem_ptr->get_size()) {
							int val = this->mem_ptr->read(index);
							ImGui::PushID(index);
							ImGui::SetNextItemWidth(30);
							ImGui::InputInt("", &val, 0, 0, ImGuiInputTextFlags_CharsHexadecimal);
							// memory[index] = val & 0xFF;
							this->mem_ptr->write(index, val&0xFF);
							ImGui::PopID();

							ascii[col] = (val >= 32 && val <= 126) ? (char)val : '.';
						} else {
							ascii[col] = '.';
							ImGui::Text("--");
						}
					}

					ImGui::TableSetColumnIndex(bytes_row + 1);
					ImGui::TextUnformatted(ascii);
				}
				ImGui::EndTable();
			}
			ImGui::End();
		}

		// Input window -> drag and drop code
		{
			ImGui::SetNextWindowPos(ImVec2(0, 200), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(250, 260), ImGuiCond_Always);
			ImGui::Begin("INPUT", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			if(file_ready){
				ImGui::Text("File: %s", dropped_file.c_str());
				ImGui::Separator();
				ImGui::BeginChild("TextScroll", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
				ImGui::TextUnformatted(file_content.c_str());
				ImGui::EndChild();
			}else{
				ImGui::Text("Arraste um arquivo aqui.");
			}
			ImGui::End();
		}

		// TODO: Output window
		// Output window -> logs events and prints final output. Example:
		// MacroProcessor: test.asm generated
		// Assembler: test.obj generated
		// Linker: test.hpx generated
		// Loader: source code loaded in memory
		// <output>
		// END OF EXECUTION
		{
			ImGui::SetNextWindowPos(ImVec2(0, 460), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(250, 260), ImGuiCond_Always);
			ImGui::Begin("OUTPUT", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::End();
		}

		// Rendering - não mexer!
		ImGui::Render();
		SDL_RenderSetScale(renderer, io->DisplayFramebufferScale.x, io->DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
	}

	// Cleanup - não mexer!
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}