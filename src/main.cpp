#include <algorithm> // min e max
#include <cassert>  // assert
#include <chrono>   // high_resolution_clock
#include <iostream> // cout
#include <numbers>  // pi
#include <string>   // string and stod
#include <tuple>
#include <vector> // vector

#include <SFML/Graphics.hpp>
#include <docopt/docopt.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

static constexpr auto USAGE = R"(C++ Weekly Game.

	Usage:
		rpg_jason [options]

	Options:
		-h --help          Show this screen.
		--width=W          Screen width in pixels [default: 1024].
		--height=H         Screen height in pixels [default: 768].
		--scale=S          Scaling percent [default: 150].
		--refresh-rate=R   Refresh rate limit [default: 60].
)";

/**
 * @brief Process the command line arguments and sets into the correct variables.
 * @param argc number of arguments in argv
 * @param argv array of arguments
 * @param width out variable of width
 * @param height out variable of height
 * @param scale out variable of scale
 * @param fps_limit out variable of fps_limit (refresh rate)
 * @return true
 */
bool get_command_options( int argc, char *argv[], unsigned int &width, unsigned int &height, float &scale,
                          double &fps_limit )
{
	std::map< std::string, docopt::value > args =
			docopt::docopt( USAGE, { std::next( argv ), std::next( argv, argc ) }, true, "Rpg Jason v0.1.0" );

	width     = args["--width"].asLong();
	height    = args["--height"].asLong();
	scale     = args["--scale"].asLong() / 100.0f;
	fps_limit = static_cast< double >( args["--refresh-rate"].asLong() );

	width     = width < 640 ? 640 : width;
	height    = height < 480 ? 480 : height;
	scale     = scale < 1.0f ? 1.0f : scale > 5.0f ? 5.0f : scale;
	fps_limit = fps_limit < 30.0 ? 30 : fps_limit > 600.0 ? 600.0 : fps_limit;

	spdlog::info( "Parameter set: --width = {}", width );
	spdlog::info( "Parameter set: --height = {}", height );
	spdlog::info( "Parameter set: --scale = {}", scale );
	spdlog::info( "Parameter set: --refresh-rate = {}", fps_limit );

	return true;
}

int main( int argc, char *argv[] )
{
	spdlog::info( "Starting..." );
	try {

		unsigned int width;
		unsigned int height;
		float        scale;
		double       fps_limit;

		if( !get_command_options( argc, argv, width, height, scale, fps_limit ) ) {
			spdlog::info( "Exiting..." );
			abort();
		}

		sf::Font font;
		if( !font.loadFromFile( "Resources/font.ttf" ) ) {
			spdlog::critical( "Failed to load font: {}", "Resources/font.ttf" );
			abort();
		}

		// const auto &video = sf::VideoMode::getDesktopMode();
		const auto video = sf::VideoMode{ width, height };

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

		ImGui::GetStyle().ScaleAllSizes( scale );
		ImGui::GetIO().FontGlobalScale = scale;

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// imgui_SFML -> doesn't implement viewport yet
		// ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_PlatformHasViewports + ImGuiBackendFlags_RendererHasViewports;
		// ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// ImFont* font1 = ImGui::GetIO().Fonts->AddFontFromFileTTF("Resources/font.ttf", 9);
		// assert(font1, "Font could not be loaded.");
		// ImGui::GetIO().Fonts->Build();
		// ImGui::PushFont(font1);

		std::vector< std::tuple< std::string, bool > > steps{
				{"The Plan",														false},
				{ "Getting Started",                    false},
				{ "Finding Errors As Soon As Possible", false},
				{ "Handling Command Line Parameter",    false},
				{ "C++ 20 So Far",                      false},
				{ "Reading SFML Input States",          false},
				{ "Managing Game State",                false},
				{ "Making Our Game Testable",           false},
				{ "Making Game State Allocator Aware",  false},
				{ "Add Logging To Game Engine",         false},
				{ "Draw A Game Map",                    false},
				{ "Dialog Trees",                       false},
				{ "Porting from SFML To SDL",           false}
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
				for( int things_count = 0; auto &[description, state]: steps ) {
					ImGui::Checkbox( fmt::format( "{} : {}", ++things_count, description ).c_str(), &state );
				}
				ImGui::End();

				window.clear( darkGrey );

				float a = 0.06f * frame;
				float t = 2.0f * std::numbers::pi_v< float > / rects.size();
				float r = initRadius * std::cos( 0.1f * a );

				for( int i = 0; auto &rect: rects ) {
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
