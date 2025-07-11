"use client";
import { useState, useRef, useEffect } from "react";

const COMMANDS = [
	{
		cmd: "help",
		desc: "Show available commands",
		exec: () =>
			`Available commands: help, clear, ls, start, quit`
	},
	{
		cmd: "clear",
		desc: "Clear the terminal",
		exec: (setLines: any) => {
			setLines([]);
			return "";
		},
	},
	{
		cmd: "ls",
		desc: "List files (demo)",
		exec: () => "README.md  arc-server/  arc-client/  data/  demo/  src/",
	},
];

function getCommand(cmd: string) {
	return COMMANDS.find((c) => c.cmd === cmd);
}

export default function Terminal() {
	const [lines, setLines] = useState<string[]>([
		"Welcome to Activity Recommender Terminal! Type 'help' to get started.",
	]);
	const [input, setInput] = useState("");
	const [mode, setMode] = useState<null | "askPeople" | "collectPrefs" | "waiting">(null);
	const [peopleCount, setPeopleCount] = useState<number>(0);
	const [preferences, setPreferences] = useState<string[][]>([]);
	const [currentPerson, setCurrentPerson] = useState(0);
	const inputRef = useRef<HTMLInputElement>(null);

	useEffect(() => {
		inputRef.current?.focus();
	}, [lines]);

	const handleCommand = async (raw: string) => {
		const cmd = raw.trim();
		if (mode === "askPeople") {
			const n = parseInt(cmd);
			if (isNaN(n) || n < 1 || n > 10) {
				setLines((l) => [...l, `Please enter a valid number (1-10):`]);
				return;
			}
			setPeopleCount(n);
			setPreferences([]);
			setCurrentPerson(0);
			setMode("collectPrefs");
			setLines((l) => [...l, `Person 1, enter your preferences (space-separated):`]);
			return;
		}
		if (mode === "collectPrefs") {
			const prefs = cmd.split(/\s+/).filter(Boolean);
			const updated = [...preferences, prefs];
			setPreferences(updated);
			if (updated.length < peopleCount) {
				setCurrentPerson(updated.length);
				setLines((l) => [...l, `Person ${updated.length + 1}, enter your preferences (space-separated):`]);
			} else {
				setMode("waiting");
				setLines((l) => [...l, `Processing...`]);
				const reqBody = {
					groupSize: peopleCount,
					preferences: updated.map((prefs, idx) => ({
						userId: `user${idx + 1}`,
						keywords: prefs
					}))
				};
				fetch(
					process.env.NODE_ENV === "development"
						? "http://localhost:6969/recommend"
						: "https://arc-server.railway.app/recommend",
					{
						method: "POST",
						headers: { "Content-Type": "application/json" },
						body: JSON.stringify(reqBody),
					}
				)
					.then((res) => res.json())
					.then((data) => {
						let result = "";
						if (data.success && Array.isArray(data.recommendations)) {
							result += `Top 3 recommended activities:\n`;
							data.recommendations.slice(0, 3).forEach((act, idx) => {
								result += `${idx + 1}. ${act.activity} (${act.matchPercentage.toFixed(2)}% match)\n`;
							});
						} else if (data.error) {
							result += `Error: ${data.error.message}`;
						} else {
							result += JSON.stringify(data);
						}
						setLines((l) => [
							...l,
							result,
							"Enter 'start' to begin or 'quit' to exit:",
						]);
						setMode(null);
					})
					.catch(() => {
						setLines((l) => [
							...l.filter((line) => line !== "Processing..."),
							"Error connecting to backend.",
						]);
						setMode(null);
					});
			}
			return;
		}
		if (cmd === "start") {
			setMode("askPeople");
			setLines((l) => [...l, "Enter number of people in the group:"]);
			return;
		}
		if (cmd === "quit") {
			setLines((l) => [...l, "Thank you for using ARC!"]);
			setMode(null);
			return;
		}
		const found = getCommand(cmd);
		if (found) {
			if (cmd === "clear") {
				found.exec(setLines);
			} else {
				setLines((l) => [...l, found.exec(setLines)]);
			}
		} else {
			setLines((l) => [...l, `Unknown command: ${cmd}`]);
		}
	};

	const handleInput = (e: React.FormEvent) => {
		e.preventDefault();
		if (!input.trim()) return;
		setLines((l) => [...l, `> ${input}`]);
		handleCommand(input);
		setInput("");
	};

	useEffect(() => {
		if (lines.length === 1) {
			setLines((l) => [...l, "Enter 'start' to begin or 'quit' to exit:"]);
		}
	}, []);
    
	const scrollRef = useRef<HTMLDivElement>(null);
	useEffect(() => {
		if (scrollRef.current) {
			scrollRef.current.scrollTop = scrollRef.current.scrollHeight;
		}
	}, [lines]);

	return (
		<div className="w-full max-w-2xl bg-gradient-to-br from-[#18181b] to-[#23272f] font-mono rounded-2xl shadow-2xl p-6 min-h-[440px] flex flex-col border border-green-700/30">
			<div className="text-center mb-4">
				<span className="text-2xl font-bold tracking-wide text-yellow-400 drop-shadow-md">Activity Recommender CLI (ARC)</span>
			</div>
			<div className="flex-1 overflow-y-auto mb-3 custom-scrollbar" style={{ minHeight: 320 }} ref={scrollRef}>
				{lines.map((line, i) => {
					let colorClass = "text-green-300";
					if (/error|invalid|failed|not found|no valid/i.test(line)) colorClass = "text-red-400 font-semibold";
					else if (/recommended|success|thank you|ready|processing/i.test(line)) colorClass = "text-green-400 font-bold";
					else if (/help|start|quit|clear|ls|command/i.test(line)) colorClass = "text-blue-400";
					else if (/enter|person|input|type/i.test(line)) colorClass = "text-yellow-300";
					else if (/unknown/i.test(line)) colorClass = "text-white";
					return (
						<div key={i} className={`whitespace-pre-wrap leading-relaxed text-[1.05rem] tracking-wide px-1 py-0.5 ${colorClass}`}>
							{line}
						</div>
					);
				})}
			</div>
			<form onSubmit={handleInput} className="flex gap-2 items-center bg-[#23272f] rounded-lg px-3 py-2 border border-green-700/20 focus-within:border-green-400 transition-all">
				<span className="text-green-400 font-bold text-lg">&gt;</span>
				<input
					ref={inputRef}
					className="flex-1 bg-transparent outline-none border-none text-green-200 placeholder:text-green-700/60 text-base px-1 py-1"
					value={input}
					onChange={(e) => setInput(e.target.value)}
					autoFocus
					disabled={mode === "waiting"}
					placeholder="Type a command..."
				/>
			</form>
			<style jsx>{`
				.custom-scrollbar::-webkit-scrollbar {
					width: 8px;
				}
				.custom-scrollbar::-webkit-scrollbar-thumb {
					background: #1a1a1a;
					border-radius: 8px;
				}
				.custom-scrollbar::-webkit-scrollbar-track {
					background: transparent;
				}
			`}</style>
		</div>
	);
}
