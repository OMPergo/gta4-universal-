import SwiftUI

@main
struct GTAIVUniversalApp: App {
    var body: some Scene {
        WindowGroup {
            LauncherView()
                .preferredColorScheme(.dark)
        }
    }
}

struct LauncherView: View {
    @State private var selectedTab = "Home"
    @State private var installStatus = "Game data not installed"
    @State private var showingInfo = false

    private let tabs = ["Home", "Install", "Settings", "Diagnostics"]

    var body: some View {
        ZStack {
            LinearGradient(
                colors: [Color(red: 0.055, green: 0.075, blue: 0.09),
                         Color(red: 0.12, green: 0.14, blue: 0.15),
                         Color.black],
                startPoint: .topLeading,
                endPoint: .bottomTrailing
            ).ignoresSafeArea()

            VStack(spacing: 0) {
                header
                ScrollView {
                    VStack(alignment: .leading, spacing: 18) {
                        if selectedTab == "Home" { home }
                        else if selectedTab == "Install" { install }
                        else if selectedTab == "Settings" { settings }
                        else { diagnostics }
                    }
                    .padding(20)
                }
                tabBar
            }
        }
        .alert("GTA IV Universal", isPresented: $showingInfo) {
            Button("OK", role: .cancel) { }
        } message: {
            Text("This is the first iOS launcher shell. The GTA IV engine and game-data launch pipeline are not integrated yet.")
        }
    }

    private var header: some View {
        HStack(alignment: .center) {
            VStack(alignment: .leading, spacing: 3) {
                Text("GTA IV UNIVERSAL")
                    .font(.system(size: 22, weight: .black, design: .rounded))
                    .tracking(1.2)
                Text("LIBERTY CITY")
                    .font(.system(size: 10, weight: .semibold))
                    .tracking(3)
                    .foregroundStyle(.white.opacity(0.55))
            }
            Spacer()
            Button { showingInfo = true } label: {
                Image(systemName: "info.circle")
                    .font(.system(size: 21))
                    .foregroundStyle(.white.opacity(0.8))
            }
        }
        .padding(.horizontal, 20)
        .padding(.top, 14)
        .padding(.bottom, 16)
        .background(.black.opacity(0.35))
    }

    private var home: some View {
        VStack(alignment: .leading, spacing: 18) {
            ZStack(alignment: .bottomLeading) {
                RoundedRectangle(cornerRadius: 18)
                    .fill(LinearGradient(colors: [.gray.opacity(0.55), .black], startPoint: .topTrailing, endPoint: .bottomLeading))
                    .frame(height: 190)
                VStack(alignment: .leading, spacing: 7) {
                    Text("WELCOME TO")
                        .font(.caption.weight(.bold))
                        .tracking(2)
                        .foregroundStyle(.white.opacity(0.7))
                    Text("LIBERTY CITY")
                        .font(.system(size: 30, weight: .black, design: .rounded))
                    Text("Your universal GTA IV launcher")
                        .font(.subheadline)
                        .foregroundStyle(.white.opacity(0.7))
                }
                .padding(20)
            }

            sectionTitle("GAME STATUS")
            statusCard

            Button { selectedTab = "Install" } label: {
                Label("INSTALL GAME DATA", systemImage: "square.and.arrow.down")
                    .font(.headline.weight(.bold))
                    .frame(maxWidth: .infinity)
                    .padding(.vertical, 16)
                    .background(Color.white)
                    .foregroundStyle(.black)
                    .clipShape(RoundedRectangle(cornerRadius: 12))
            }

            Button { showingInfo = true } label: {
                Label("PLAY", systemImage: "play.fill")
                    .font(.headline.weight(.bold))
                    .frame(maxWidth: .infinity)
                    .padding(.vertical, 16)
                    .background(.white.opacity(0.10))
                    .foregroundStyle(.white.opacity(0.65))
                    .clipShape(RoundedRectangle(cornerRadius: 12))
            }
        }
    }

    private var statusCard: some View {
        VStack(alignment: .leading, spacing: 10) {
            Label(installStatus, systemImage: "externaldrive")
                .font(.subheadline.weight(.semibold))
            Divider().overlay(.white.opacity(0.15))
            HStack {
                metric("PLATFORM", "iOS")
                Spacer()
                metric("BUILD", "Preview")
                Spacer()
                metric("ENGINE", "Pending")
            }
        }
        .padding(16)
        .background(.white.opacity(0.07))
        .clipShape(RoundedRectangle(cornerRadius: 14))
    }

    private var install: some View {
        VStack(alignment: .leading, spacing: 16) {
            sectionTitle("GAME INSTALLATION")
            Text("Game files must be supplied by you. This preview does not download or contain Rockstar game data.")
                .foregroundStyle(.white.opacity(0.7))
                .font(.subheadline)
            statusCard
            Label("Base game image — not configured", systemImage: "opticaldisc")
            Label("Title Update 8 — not configured", systemImage: "arrow.down.circle")
            Text("The iOS file-import and extraction pipeline still needs to be implemented.")
                .font(.footnote)
                .foregroundStyle(.yellow.opacity(0.85))
        }
        .padding(18)
        .background(.white.opacity(0.06))
        .clipShape(RoundedRectangle(cornerRadius: 14))
    }

    private var settings: some View {
        VStack(alignment: .leading, spacing: 16) {
            sectionTitle("SETTINGS")
            settingRow("Graphics", value: "Native (default)")
            settingRow("Frame limit", value: "30 FPS")
            settingRow("Anti-aliasing", value: "FXAA")
            settingRow("Renderer", value: "iOS backend pending")
            Text("These are preview values; settings are not connected to the game engine yet.")
                .font(.footnote)
                .foregroundStyle(.white.opacity(0.55))
        }
        .padding(18)
        .background(.white.opacity(0.06))
        .clipShape(RoundedRectangle(cornerRadius: 14))
    }

    private var diagnostics: some View {
        VStack(alignment: .leading, spacing: 16) {
            sectionTitle("DIAGNOSTICS")
            settingRow("App", value: "iOS launcher shell")
            settingRow("Native engine", value: "Not integrated")
            settingRow("Game process", value: "Unavailable")
            settingRow("Crash capture", value: "Not implemented")
            Text("This screen will display real runtime, memory, renderer, and crash information after native integration.")
                .font(.footnote)
                .foregroundStyle(.white.opacity(0.55))
        }
        .padding(18)
        .background(.white.opacity(0.06))
        .clipShape(RoundedRectangle(cornerRadius: 14))
    }

    private var tabBar: some View {
        HStack(spacing: 0) {
            ForEach(tabs, id: \.self) { tab in
                Button { selectedTab = tab } label: {
                    VStack(spacing: 5) {
                        Image(systemName: icon(for: tab))
                        Text(tab).font(.system(size: 10, weight: .semibold))
                    }
                    .foregroundStyle(selectedTab == tab ? .white : .white.opacity(0.45))
                    .frame(maxWidth: .infinity)
                    .padding(.vertical, 12)
                }
            }
        }
        .background(.black.opacity(0.55))
    }

    private func sectionTitle(_ title: String) -> some View {
        Text(title).font(.caption.weight(.heavy)).tracking(2).foregroundStyle(.white.opacity(0.65))
    }

    private func metric(_ title: String, _ value: String) -> some View {
        VStack(alignment: .leading, spacing: 4) {
            Text(title).font(.system(size: 9, weight: .bold)).foregroundStyle(.white.opacity(0.45))
            Text(value).font(.system(size: 13, weight: .semibold))
        }
    }

    private func settingRow(_ title: String, value: String) -> some View {
        HStack {
            Text(title).foregroundStyle(.white.opacity(0.75))
            Spacer()
            Text(value).font(.subheadline.weight(.semibold))
        }
        .padding(.vertical, 7)
    }

    private func icon(for tab: String) -> String {
        switch tab {
        case "Home": return "house.fill"
        case "Install": return "square.and.arrow.down"
        case "Settings": return "slider.horizontal.3"
        default: return "waveform.path.ecg"
        }
    }
}
