import SwiftUI
import UniformTypeIdentifiers
import UIKit

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
    @AppStorage("gameDataFolderName") private var gameDataFolderName = ""
    @AppStorage("graphicsPreset") private var graphicsPreset = "Native"
    @AppStorage("frameLimit") private var frameLimit = "30 FPS"
    @AppStorage("antiAliasing") private var antiAliasing = "FXAA"

    @State private var selectedTab = "Home"
    @State private var showingFolderPicker = false
    @State private var showingInfo = false
    @State private var importMessage = ""
    @State private var importError = false

    private let tabs = ["Home", "Install", "Settings", "Diagnostics"]

    private var gameDataURL: URL? {
        guard !gameDataFolderName.isEmpty,
              let documents = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first else {
            return nil
        }
        return documents.appendingPathComponent(gameDataFolderName, isDirectory: true)
    }

    private var hasGameData: Bool {
        guard let url = gameDataURL else { return false }
        var isDirectory: ObjCBool = false
        return FileManager.default.fileExists(atPath: url.path, isDirectory: &isDirectory) && isDirectory.boolValue
    }

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
        .fileImporter(
            isPresented: $showingFolderPicker,
            allowedContentTypes: [.folder],
            allowsMultipleSelection: false,
            onCompletion: handleFolderImport
        )
        .alert("GTA IV Universal", isPresented: $showingInfo) {
            Button("OK", role: .cancel) { }
        } message: {
            Text("This is a functional iOS launcher and game-folder manager. The GTA IV native engine and gameplay launch pipeline are not integrated yet.")
        }
        .alert(importError ? "Import failed" : "Game folder imported", isPresented: Binding(
            get: { !importMessage.isEmpty },
            set: { if !$0 { importMessage = "" } }
        )) {
            Button("OK", role: .cancel) { importMessage = "" }
        } message: {
            Text(importMessage)
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
            .accessibilityLabel("About GTA IV Universal")
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
                Label(hasGameData ? "MANAGE GAME FOLDER" : "IMPORT GAME FOLDER", systemImage: "folder.badge.plus")
                    .font(.headline.weight(.bold))
                    .frame(maxWidth: .infinity)
                    .padding(.vertical, 16)
                    .background(Color.white)
                    .foregroundStyle(.black)
                    .clipShape(RoundedRectangle(cornerRadius: 12))
            }

            Label("GAME ENGINE NOT CONNECTED", systemImage: "info.circle")
                .font(.caption.weight(.bold))
                .foregroundStyle(.yellow.opacity(0.9))
                .frame(maxWidth: .infinity, alignment: .center)
                .padding(.top, 4)
        }
    }

    private var statusCard: some View {
        VStack(alignment: .leading, spacing: 10) {
            Label(hasGameData ? "Game folder imported" : "Game data not installed", systemImage: hasGameData ? "checkmark.circle.fill" : "externaldrive")
                .font(.subheadline.weight(.semibold))
            if hasGameData, let url = gameDataURL {
                Text(url.lastPathComponent)
                    .font(.caption)
                    .foregroundStyle(.white.opacity(0.55))
                    .lineLimit(1)
            }
            Divider().overlay(.white.opacity(0.15))
            HStack {
                metric("PLATFORM", "iOS")
                Spacer()
                metric("BUILD", appVersion)
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
            Text("Select a folder containing your legally obtained GTA IV game files. The app copies it into its own private storage; it does not download or include Rockstar game data.")
                .foregroundStyle(.white.opacity(0.7))
                .font(.subheadline)
            statusCard
            Button { showingFolderPicker = true } label: {
                Label("CHOOSE GAME FOLDER", systemImage: "folder")
                    .font(.headline.weight(.bold))
                    .frame(maxWidth: .infinity)
                    .padding(.vertical, 15)
                    .background(Color.white)
                    .foregroundStyle(.black)
                    .clipShape(RoundedRectangle(cornerRadius: 12))
            }
            if hasGameData {
                Label("Your imported folder is saved inside this app.", systemImage: "checkmark.shield")
                    .font(.footnote)
                    .foregroundStyle(.green.opacity(0.9))
            }
            Text("Import and storage management work in this build. Game-file validation, extraction of disc images, and launching the native game engine are not implemented yet.")
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
            settingPicker("Graphics preset", selection: $graphicsPreset, options: ["Native", "Low", "Medium", "High"])
            settingPicker("Frame limit", selection: $frameLimit, options: ["30 FPS", "40 FPS", "60 FPS"])
            settingPicker("Anti-aliasing", selection: $antiAliasing, options: ["Off", "FXAA", "MSAA"])
            Text("These preferences are saved on this device. They will be passed to the game engine once its iOS integration is complete.")
                .font(.footnote)
                .foregroundStyle(.white.opacity(0.55))
        }
        .padding(18)
        .background(.white.opacity(0.06))
        .clipShape(RoundedRectangle(cornerRadius: 14))
    }

    private var diagnostics: some View {
        VStack(alignment: .leading, spacing: 16) {
            sectionTitle("DEVICE DIAGNOSTICS")
            settingRow("App", value: "GTA IV Universal")
            settingRow("App version", value: appVersion)
            settingRow("Device", value: UIDevice.current.model)
            settingRow("iOS version", value: UIDevice.current.systemVersion)
            settingRow("Game folder", value: hasGameData ? "Imported" : "Not imported")
            settingRow("Native engine", value: "Not integrated")
            settingRow("Game process", value: "Not available")
            Text("Device and app information is read from iOS. Runtime, renderer, memory, and crash diagnostics will become available after native engine integration.")
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
                .accessibilityLabel(tab)
            }
        }
        .background(.black.opacity(0.55))
    }

    private var appVersion: String {
        let version = Bundle.main.infoDictionary?["CFBundleShortVersionString"] as? String ?? "1.0"
        let build = Bundle.main.infoDictionary?["CFBundleVersion"] as? String ?? "1"
        return "\(version) (\(build))"
    }

    private func handleFolderImport(_ result: Result<[URL], Error>) {
        switch result {
        case .failure(let error):
            importError = true
            importMessage = error.localizedDescription
        case .success(let urls):
            guard let source = urls.first else { return }
            let hasAccess = source.startAccessingSecurityScopedResource()
            defer { if hasAccess { source.stopAccessingSecurityScopedResource() } }
            do {
                guard let documents = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first else {
                    throw NSError(domain: "GTAIVUniversal", code: 1, userInfo: [NSLocalizedDescriptionKey: "App storage is unavailable."])
                }
                let baseName = "GameData"
                var destination = documents.appendingPathComponent(baseName, isDirectory: true)
                var suffix = 2
                while FileManager.default.fileExists(atPath: destination.path) {
                    destination = documents.appendingPathComponent("\(baseName)-\(suffix)", isDirectory: true)
                    suffix += 1
                }
                try FileManager.default.copyItem(at: source, to: destination)
                gameDataFolderName = destination.lastPathComponent
                importError = false
                importMessage = "Your game folder was copied into the app's private storage as \(destination.lastPathComponent). The game engine is not connected yet."
            } catch {
                importError = true
                importMessage = error.localizedDescription
            }
        }
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

    private func settingPicker(_ title: String, selection: Binding<String>, options: [String]) -> some View {
        VStack(alignment: .leading, spacing: 7) {
            Text(title).foregroundStyle(.white.opacity(0.75))
            Picker(title, selection: selection) {
                ForEach(options, id: \.self) { option in
                    Text(option).tag(option)
                }
            }
            .pickerStyle(.menu)
            .tint(.white)
            .frame(maxWidth: .infinity, alignment: .leading)
            .padding(.horizontal, 12)
            .padding(.vertical, 8)
            .background(.white.opacity(0.08))
            .clipShape(RoundedRectangle(cornerRadius: 10))
        }
    }

    private func settingRow(_ title: String, value: String) -> some View {
        HStack {
            Text(title).foregroundStyle(.white.opacity(0.75))
            Spacer()
            Text(value).font(.subheadline.weight(.semibold))
                .multilineTextAlignment(.trailing)
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
