bool open = true;

void main()
{
	print("ImGUI example");
}

// Game entry point - called every frame. Argument is delta time in seconds.
void frameStep(float dt)
{
	ImGui::Begin("AS ImGUI window", open);

	ImGui::Text("This window was created from AngelScipt");
	ImGui::Separator();

	ImGui::Button("Cool");

	ImGui::End();
}
