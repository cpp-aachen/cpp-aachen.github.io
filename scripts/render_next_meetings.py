#!/usr/bin/env python3
import json
import sys
from datetime import datetime
from zoneinfo import ZoneInfo
from pathlib import Path

CAL_PATH = Path("calendar/cpp-aachen.jsoncal")
README_SRC = Path("README.md")
README_OUT = Path("README.md")  # überschreibt im Arbeitsverzeichnis (nur im Build!)
PLACEHOLDER = "{{ NEXT_MEETINGS }}"
TZ = ZoneInfo("Europe/Berlin")

def load_events():
    with CAL_PATH.open("r", encoding="utf-8") as f:
        data = json.load(f)
    events = data.get("events", [])
    # parse start as aware datetime in specified timezone (if provided), otherwise naive->Berlin
    norm_events = []
    for e in events:
        start_str = e.get("start")
        if not start_str:
            continue
        tzname = e.get("timezone")
        tz = ZoneInfo(tzname) if tzname else TZ
        try:
            # ISO 8601 string -> naive dt; assign tz for proper comparison and formatting
            dt = datetime.fromisoformat(start_str)
            if dt.tzinfo is None:
                dt = dt.replace(tzinfo=tz)
            else:
                # normalize to event tz if provided
                if tzname:
                    dt = dt.astimezone(tz)
        except Exception:
            continue
        norm_events.append({
            "summary": e.get("summary", "C++ User Group Aachen"),
            "location": e.get("location", ""),
            "description": e.get("description", ""),
            "url": e.get("url", ""),
            "start": dt,
            "tz": tz
        })
    return norm_events

def filter_future(events):
    now = datetime.now(TZ)
    return [e for e in events if e["start"] >= now]

def format_markdown(events, limit=None):
    if not events:
        return "_Aktuell sind keine zukünftigen Termine geplant._"
    # optional: limit anzeigen (z.B. Top 6)
    if limit:
        events = events[:limit]
    lines = []
    # Markdown-Format: - Mi, 3. Dezember 2025, 18:30 — Utimaco, Aachen — Kurzbeschreibung
    # inklusive Link, wenn vorhanden
    for e in events:
        dt = e["start"]
        date_str = dt.strftime("%A, %-d. %B %Y, %H:%M").replace("Monday","Montag")\
                                                      .replace("Tuesday","Dienstag")\
                                                      .replace("Wednesday","Mittwoch")\
                                                      .replace("Thursday","Donnerstag")\
                                                      .replace("Friday","Freitag")\
                                                      .replace("Saturday","Samstag")\
                                                      .replace("Sunday","Sonntag")
        # Einige Systeme unterstützen %-d nicht (Mac). Fallback:
        if "%-d" in "%-d":
            date_str = dt.strftime("%A, %d. %B %Y, %H:%M")
            # führende Null optional entfernen:
            if date_str[9] == "0":
                date_str = date_str[:9] + date_str[10:]

        parts = [f"{date_str}"]
        if e["location"]:
            parts.append(e["location"])
        desc = e["description"].strip() if e["description"] else ""
        # Kurzbeschreibung eine Zeile
        if desc:
            # Nur erste Zeile/verkürzt
            short = desc.splitlines()[0]
            if len(short) > 120:
                short = short[:117] + "…"
            parts.append(short)
        line = " — ".join(parts)
        # Link optional
        if e["url"]:
            line += f" — {e['url']}"
        lines.append(f"- {line}")
    return "\n".join(lines)

def main():
    events = load_events()
    events = sorted(events, key=lambda e: e["start"])
    future = filter_future(events)
    md = format_markdown(future, limit=10)

    text = README_SRC.read_text(encoding="utf-8")
    if PLACEHOLDER not in text:
        print(f"Warnung: Platzhalter {PLACEHOLDER} nicht in README gefunden.", file=sys.stderr)
    text = text.replace(PLACEHOLDER, md)
    README_OUT.write_text(text, encoding="utf-8")

if __name__ == "__main__":
    main()
