#include <algorithm> // min e max
#include <array>
#include <cassert>  // assert
#include <chrono>   // high_resolution_clock
#include <iostream> // cout
#include <numbers>  // pi
#include <string>   // string and stod
#include <vector>   // vector
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

int main( [[maybe_unused]] int argc, [[maybe_unused]] char *argv[] )
{
	spdlog::info( "Starting..." );
	try {

		[[maybe_unused]] std::vector< std::string > args( argv, argv + argc );

		double fps_limit = args.size() > 1 ? std::stod( args[1] ) : 60;

		spdlog::info( "fps_limit: {}", fps_limit );

		sf::Font font;
		if( !font.loadFromFile( "Resources/font.ttf" ) ) {
			spdlog::critical( "Failed to load font: {}", "Resources/font.ttf" );
			return 1;
		}

		// const auto &video = sf::VideoMode::getDesktopMode();
		const auto video = sf::VideoMode{ 1024, 768 };

		sf::RenderWindow window( video, "SFML + ImGui = <3" );
		sf::Color        darkGrey{ 40, 40, 40, 255 };

		std::vector< sf::RectangleShape > rects;
		std::vector< sf::Color >          colors{
        sf::Color::Red,  sf::Color::Green,        sf::Color::Blue,         sf::Color::Yellow,       sf::Color::Magenta,
        sf::Color::Cyan, sf::Color( 0x3b1e08ff ), sf::Color( 0xfc6a03ff ), sf::Color( 0xaf69eeff ),
    };
		float        size = 180.0f / colors.size() / 640.0f * video.width;
		sf::Vector2f sz{ size, size };

		for( auto &color: colors ) {
			sf::RectangleShape rect( sz );
			rect.setFillColor( color );
			rects.push_back( rect );
		}

		sf::Text text;
		text.setFont( font );
		text.setCharacterSize( 24 );
		text.setFillColor( sf::Color::White );

		auto        t0    = std::chrono::high_resolution_clock::now();
		std::size_t frame = 0;

		float startx     = 290.0f / 640.0f * video.width;
		float starty     = 240.0f / 480.0f * video.height;
		float initRadius = 100.0f / 640.0f * video.width;

		if( !ImGui::SFML::Init( window, true ) ) {
			spdlog::critical( "Impossível inicializar ImGui com SFML..." );
			return 2;
		}

		constexpr auto scale_factor = 1.5;
		ImGui::GetStyle().ScaleAllSizes( scale_factor );
		ImGui::GetIO().FontGlobalScale = scale_factor;

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// imgui_SFML -> doesn't implement viewport yet
		// ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_PlatformHasViewports + ImGuiBackendFlags_RendererHasViewports;
		// ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// ImFont* font1 = ImGui::GetIO().Fonts->AddFontFromFileTTF("Resources/font.ttf", 9);
		// assert(font1, "Font could not be loaded.");
		// ImGui::GetIO().Fonts->Build();
		// ImGui::PushFont(font1);

		std::unordered_map<std::string, bool> set_of_things {
				{"The Plan", false},
				{"Getting Started", false},
				{"Finding Errors As Soon As Possible", false},
				{"Handling Command Line PArameter", false},
				{"C++ 20 So Far", false},
				{"Reading SFML Input States", false},
				{"Managing Game State", false},
				{"Making Our Game Testable", false},
				{"Making Game State Allocator Aware", false},
				{"Add Logging To Game Engine", false},
				{"Draw A Game Map", false},
				{"Dialog Trees", false},
				{"Porting from SFML To SDL", false}
		};

		sf::Clock deltaClock;

		for( ;; ) {

			sf::Event event;

			while( window.pollEvent( event ) ) {

				ImGui::SFML::ProcessEvent( event );

				switch( event.type ) {
					case sf::Event::Closed:
						window.close();
						break;
					case sf::Event::KeyPressed:
						if( event.key.code == sf::Keyboard::Escape ) {
							window.close();
						}
						break;
					default:
						break;
				}
			}
			if( !window.isOpen() )
				break;

			auto   t1  = std::chrono::high_resolution_clock::now();
			auto   dt  = std::chrono::duration_cast< std::chrono::nanoseconds >( t1 - t0 ).count();
			double fps = 1e9 / dt;

			if( fps <= fps_limit ) {
				t0 = t1;

				ImGui::SFML::Update( window, deltaClock.restart() );

				ImGui::ShowDemoWindow();

				ImGui::Begin( "The Plan" );
				int things_count = 0;
				for(auto &[description, state] : set_of_things) {
					ImGui::Checkbox(fmt::format("{} : {}", ++things_count, description).c_str(), &state);
				}
				ImGui::End();

				window.clear( darkGrey );

				float a = 0.06f * frame;
				float t = 2.0f * std::numbers::pi_v< float > / rects.size();
				float r = initRadius * std::cos( 0.1f * a );

				int i = 0;
				for( auto &rect: rects ) {
					const float ati = a + t * i;
					rect.setPosition( { startx + r * std::cos( ati ), starty + r * std::sin( ati ) } );
					window.draw( rect );
					i++;
				}

				text.setString( fmt::format( "FPS: {:.3} - Squares: {}", fps, rects.size() ) );
				window.draw( text );

				// For some reason OpenGL States must be preserved from ImGui.
				window.pushGLStates();
				// window.resetGLStates();
				ImGui::SFML::Render();
				// window.resetGLStates();
				window.popGLStates();

				window.display();
				frame++;
			}
		}

		ImGui::SFML::Shutdown();

		spdlog::info( "Exiting..." );
	} catch( const std::exception &e ) {
		spdlog::critical( "Fatal Exception: {}", e.what() );
	}

	return 0;
}
