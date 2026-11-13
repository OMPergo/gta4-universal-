package com.libertyrecomp;

import android.app.Activity;
import android.content.Context;
import android.net.DhcpInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.SystemClock;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public final class MultiplayerActivity extends Activity {
    private static final int DISCOVERY_PORT = 27888;
    private static final int PROTOCOL_MAGIC = 0x4C43554D;
    private static final int PROTOCOL_VERSION = 1;
    private static final int DISCOVERY_PROBE = 1;
    private static final int DISCOVERY_RESPONSE = 2;
    private static final long REFRESH_INTERVAL_MS = 5000L;
    private static final long SCAN_WINDOW_MS = 1800L;

    private final Handler handler = new Handler(Looper.getMainLooper());
    private final ExecutorService scanner = Executors.newSingleThreadExecutor();
    private TextView networkSummary;
    private TextView onlineSummary;
    private TextView lanSummary;
    private TextView sessionSummary;
    private LinearLayout serverList;
    private Button refreshButton;
    private boolean scanning;
    private boolean resumed;
    private int sequence;

    private final Runnable periodicRefresh = new Runnable() {
        @Override public void run() {
            if (!resumed) return;
            refreshNetworkStatus();
            if (!scanning) scanLan();
            handler.postDelayed(this, REFRESH_INTERVAL_MS);
        }
    };

    @Override protected void onCreate(Bundle state) {
        super.onCreate(state);
        buildPage();
        refreshNetworkStatus();
        scanLan();
    }

    @Override protected void onResume() {
        super.onResume();
        resumed = true;
        handler.removeCallbacks(periodicRefresh);
        handler.postDelayed(periodicRefresh, REFRESH_INTERVAL_MS);
    }

    @Override protected void onPause() {
        resumed = false;
        handler.removeCallbacks(periodicRefresh);
        super.onPause();
    }

    @Override protected void onDestroy() {
        handler.removeCallbacksAndMessages(null);
        scanner.shutdownNow();
        super.onDestroy();
    }

    private void buildPage() {
        ScrollView scroll = new ScrollView(this);
        scroll.setFillViewport(true);
        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setPadding(dp(22), dp(18), dp(22), dp(28));
        root.setBackgroundColor(0xFF080808);
        scroll.addView(root);

        TextView title = text("MULTIPLAYER", 25, 0xFFF4F1E8, true);
        root.addView(title);
        TextView subtitle = text("Liberty City Universal network hub", 14, 0xFFAAA9A5, false);
        root.addView(subtitle, params(0, dp(34)));

        networkSummary = addSection(root, "NETWORK STATUS",
                "Checking active network…");
        onlineSummary = addSection(root, "ONLINE SERVERS",
                "Community server directory is not connected yet.");
        lanSummary = addSection(root, "LOCAL NETWORK (LAN)",
                "Preparing local discovery…");
        sessionSummary = addSection(root, "YOUR SESSION",
                "No multiplayer session is active.");

        TextView listTitle = text("DISCOVERED LAN SESSIONS", 15, 0xFFF4F1E8, true);
        root.addView(listTitle, params(0, dp(12)));
        serverList = new LinearLayout(this);
        serverList.setOrientation(LinearLayout.VERTICAL);
        root.addView(serverList);

        refreshButton = new Button(this);
        refreshButton.setText("REFRESH NETWORK");
        refreshButton.setOnClickListener(v -> {
            refreshNetworkStatus();
            scanLan();
        });
        root.addView(refreshButton, params(0, dp(52)));

        Button back = new Button(this);
        back.setText("BACK TO GAME MENU");
        back.setOnClickListener(v -> finish());
        root.addView(back, params(0, dp(52)));

        TextView note = text(
                "Discovery checks for compatible LCUM LAN beacons every 5 seconds while this page is open. "
                + "Online directory, direct joining, and session hosting will appear when their transport services are connected.",
                12, 0xFFAAA9A5, false);
        root.addView(note, params(0, dp(18)));
        setContentView(scroll);
    }

    private TextView addSection(LinearLayout root, String heading, String value) {
        TextView label = text(heading, 13, 0xFFAAA9A5, true);
        root.addView(label, params(0, dp(16)));
        TextView body = text(value, 15, 0xFFF4F1E8, false);
        body.setPadding(dp(12), dp(12), dp(12), dp(12));
        body.setBackgroundColor(0xFF151515);
        root.addView(body, params(0, dp(14)));
        return body;
    }

    private TextView text(String value, int size, int color, boolean bold) {
        TextView view = new TextView(this);
        view.setText(value);
        view.setTextSize(size);
        view.setTextColor(color);
        if (bold) view.setTypeface(null, android.graphics.Typeface.BOLD);
        return view;
    }

    private LinearLayout.LayoutParams params(int width, int top) {
        LinearLayout.LayoutParams p = new LinearLayout.LayoutParams(
                width == 0 ? LinearLayout.LayoutParams.MATCH_PARENT : width,
                LinearLayout.LayoutParams.WRAP_CONTENT);
        p.topMargin = top;
        p.bottomMargin = dp(8);
        return p;
    }

    private int dp(int value) {
        return (int) (value * getResources().getDisplayMetrics().density + 0.5f);
    }

    private void refreshNetworkStatus() {
        String type = "Network status unavailable";
        String address = "Local IP unavailable";
        String broadcast = "Broadcast unavailable";
        try {
            android.net.ConnectivityManager cm =
                    (android.net.ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
            android.net.Network active = cm == null ? null : cm.getActiveNetwork();
            android.net.NetworkCapabilities caps =
                    active == null || cm == null ? null : cm.getNetworkCapabilities(active);
            if (caps != null) {
                if (caps.hasTransport(android.net.NetworkCapabilities.TRANSPORT_WIFI)) type = "Wi-Fi";
                else if (caps.hasTransport(android.net.NetworkCapabilities.TRANSPORT_ETHERNET)) type = "Ethernet";
                else if (caps.hasTransport(android.net.NetworkCapabilities.TRANSPORT_CELLULAR)) type = "Mobile data";
                else type = "Connected network";
            } else {
                type = "Offline";
            }

            WifiManager wifi = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
            DhcpInfo dhcp = wifi == null ? null : wifi.getDhcpInfo();
            if (dhcp != null && dhcp.ipAddress != 0) {
                address = intToAddress(dhcp.ipAddress).getHostAddress();
                int mask = dhcp.netmask;
                if (mask != 0) broadcast = intToAddress((dhcp.ipAddress & mask) | ~mask).getHostAddress();
            }
        } catch (Exception e) {
            type = "Network details unavailable: " + e.getClass().getSimpleName();
        }
        networkSummary.setText(type + "\nDevice address: " + address + "\nLAN broadcast: " + broadcast);
        onlineSummary.setText("Status: Not connected\nNo official/community server directory endpoint is configured in this build.");
        sessionSummary.setText("Status: Not hosting or joined\nSession transport is not connected to the game runtime yet.");
    }

    private InetAddress intToAddress(int value) throws Exception {
        byte[] bytes = new byte[] {
                (byte) (value & 0xff), (byte) ((value >> 8) & 0xff),
                (byte) ((value >> 16) & 0xff), (byte) ((value >> 24) & 0xff)
        };
        return InetAddress.getByAddress(bytes);
    }

    private void scanLan() {
        if (scanning || scanner.isShutdown()) return;
        scanning = true;
        if (refreshButton != null) refreshButton.setEnabled(false);
        lanSummary.setText("Scanning local network for LCUM-compatible hosts…");
        scanner.execute(() -> {
            List<String> found = discoverLanServers();
            runOnUiThread(() -> {
                scanning = false;
                if (refreshButton != null) refreshButton.setEnabled(true);
                lanSummary.setText("Last scan complete • " + found.size()
                        + " compatible session(s) found • refreshes every 5 seconds");
                serverList.removeAllViews();
                if (found.isEmpty()) {
                    TextView empty = text("No compatible LAN sessions found. Make sure another device is hosting and both devices are on the same Wi-Fi network.", 14, 0xFFAAA9A5, false);
                    empty.setPadding(dp(12), dp(10), dp(12), dp(10));
                    serverList.addView(empty);
                } else {
                    for (String row : found) {
                        TextView item = text(row, 14, 0xFFF4F1E8, false);
                        item.setPadding(dp(12), dp(10), dp(12), dp(10));
                        item.setBackgroundColor(0xFF151515);
                        serverList.addView(item, params(0, dp(4)));
                    }
                }
            });
        });
    }

    private List<String> discoverLanServers() {
        Map<String, String> unique = new LinkedHashMap<>();
        DatagramSocket socket = null;
        try {
            socket = new DatagramSocket();
            socket.setBroadcast(true);
            socket.setSoTimeout(250);
            byte[] probe = ByteBuffer.allocate(12).order(ByteOrder.LITTLE_ENDIAN)
                    .putInt(PROTOCOL_MAGIC).putShort((short) PROTOCOL_VERSION)
                    .put((byte) DISCOVERY_PROBE).put((byte) 0)
                    .putInt(++sequence).array();

            List<InetAddress> targets = new ArrayList<>();
            targets.add(InetAddress.getByName("255.255.255.255"));
            try {
                WifiManager wifi = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
                DhcpInfo dhcp = wifi == null ? null : wifi.getDhcpInfo();
                if (dhcp != null && dhcp.ipAddress != 0 && dhcp.netmask != 0) {
                    InetAddress subnetBroadcast = intToAddress((dhcp.ipAddress & dhcp.netmask) | ~dhcp.netmask);
                    if (!targets.contains(subnetBroadcast)) targets.add(subnetBroadcast);
                }
            } catch (Exception ignored) { }

            for (InetAddress target : targets) {
                byte[] bytes = probe.clone();
                socket.send(new DatagramPacket(bytes, bytes.length, target, DISCOVERY_PORT));
            }

            long deadline = SystemClock.elapsedRealtime() + SCAN_WINDOW_MS;
            while (!Thread.currentThread().isInterrupted() && SystemClock.elapsedRealtime() < deadline) {
                byte[] buffer = new byte[256];
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                try {
                    socket.receive(packet);
                    String parsed = parseBeacon(packet);
                    if (parsed != null) unique.put(packet.getAddress().getHostAddress() + parsed, parsed);
                } catch (SocketTimeoutException timeout) {
                    // Continue until the bounded scan window expires.
                }
            }
        } catch (Exception e) {
            unique.put("scan-error", "LAN scan unavailable: " + e.getClass().getSimpleName());
        } finally {
            if (socket != null) socket.close();
        }
        return new ArrayList<>(unique.values());
    }

    private String parseBeacon(DatagramPacket packet) {
        if (packet.getLength() < 52) return null;
        ByteBuffer data = ByteBuffer.wrap(packet.getData(), packet.getOffset(), packet.getLength())
                .order(ByteOrder.LITTLE_ENDIAN);
        if (data.getInt(0) != PROTOCOL_MAGIC || (data.getShort(4) & 0xffff) != PROTOCOL_VERSION
                || (data.get(6) & 0xff) != DISCOVERY_RESPONSE) return null;
        int port = data.getShort(12) & 0xffff;
        if (port == 0) return null;
        byte[] nameBytes = new byte[32];
        data.position(14);
        data.get(nameBytes);
        String name = new String(nameBytes, StandardCharsets.UTF_8).replace("\u0000", "").trim();
        int players = data.get(46) & 0xff;
        int maxPlayers = data.get(47) & 0xff;
        long revision = data.getInt(48) & 0xffffffffL;
        if (name.isEmpty()) name = "Liberty City session";
        return name + "\n" + packet.getAddress().getHostAddress() + ":" + port
                + " • Players " + players + "/" + maxPlayers + " • Build " + revision;
    }
}
