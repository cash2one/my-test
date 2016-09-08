package com.chanct.gms.listenser;

import java.io.File;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.chanct.gms.config.Global;
import com.chanct.gms.parser.UnPackParser;

/**
 * 
 * @ClassName: DirectoryWatcher
 * @Description: 目录看守
 * @author shf
 * @date Dec 9, 2013 4:34:58 PM
 * 
 */
public class DirectoryWatcher extends AbstractResourceWatcher {
	/** 日志 */
	private static final Log log = LogFactory.getLog(DirectoryWatcher.class);
	/**
	 * The current map of files and their timestamps (String fileName => Long
	 * lastMod)
	 */
	@SuppressWarnings("unchecked")
	private Map currentFiles = new HashMap();

	/** 解包解析器 */
	private UnPackParser unPackParser;

	/**
	 * The directory to watch.
	 */
	private String directory;

	/**
	 * The map of last recorded files and their timestamps (String fileName =>
	 * Long lastMod)
	 */
	@SuppressWarnings("unchecked")
	private Map prevFiles = new HashMap();

	/**
	 * Constructor that takes the directory to watch.
	 * 
	 * @param directoryPath
	 *            the directory to watch
	 * @param intervalSeconds
	 *            The interval to use when monitoring this directory. I.e., ever
	 *            x seconds, check this directory to see what has changed.
	 * @throws IllegalArgumentException
	 *             if the argument does not map to a valid directory
	 */
	public DirectoryWatcher(String directoryPath, int intervalSeconds)
			throws IllegalArgumentException {
		// Get the common thread interval stuff set up.
		super(intervalSeconds, directoryPath + " interval watcher.");

		// Check that it is indeed a directory.
		File theDirectory = new File(directoryPath);

		if (theDirectory != null && !theDirectory.isDirectory()) {
			// This is bad, so let the caller know
			String message = "The path " + directory
					+ " does not represent a valid directory.";
			throw new IllegalArgumentException(message);
		}
		// Else all is well so set this directory and the interval
		this.directory = directoryPath;
	}

	/**
	 * Start the monitoring of this directory.
	 */
	@SuppressWarnings("unchecked")
	public void start() {
		// Since we're going to start monitoring, we want to take a snapshot of
		// the
		// current directory to we have something to refer to when stuff
		// changes.
		takeSnapshot();
		// 若当前目录下有升级包，则进行解包
		if (currentFiles.size() > 0) {
			for (Object obj : currentFiles.entrySet()) {
				Map.Entry entry = (Map.Entry) obj;
				String version = entry.getKey().toString().split("-")[1];
				Global.packMap.put(version, entry.getKey().toString());
				try {
					unPackParser.packFormatValid(entry.getKey().toString());
					unPackParser.unPack(entry.getKey().toString());
					Global.unPackMap.put(version, Global.UNPACKING_DIRECTORY
							+ version + "/");
				} catch (Exception e) {
					if (log.isInfoEnabled()) {
						log.info("升级目录看守异常. 原因: " + e.getMessage());
					}
					continue;
				} finally {
					Global.packMap.remove(entry.getKey().toString());
				}
			}
		}
		// And start the thread on the given interval
		super.start();
		// And notify the listeners that monitoring has started
		File theDirectory = new File(directory);
		monitoringStarted(theDirectory);
	}

	/**
	 * Stop the monitoring of this directory.
	 */
	public void stop() {
		// And start the thread on the given interval
		super.stop();
		// And notify the listeners that monitoring has started
		File theDirectory = new File(directory);
		monitoringStopped(theDirectory);
	}

	/**
	 * Store the file names and the last modified timestamps of all the files
	 * and directories that exist in the directory at this moment.
	 */
	@SuppressWarnings("unchecked")
	private void takeSnapshot() {
		// Set the last recorded snap shot to be the current list
		prevFiles.clear();
		prevFiles.putAll(currentFiles);

		// And get a new current state with all the files and directories
		currentFiles.clear();

		File theDirectory = new File(directory);
		File[] children = theDirectory.listFiles();

		// Store all the current files and their timestamps
		for (int i = 0; i < children.length; i++) {
			File file = children[i];
			currentFiles.put(file.getAbsolutePath(), new Long(file
					.lastModified()));
		}
	}

	/**
	 * Check this directory for any changes and fire the proper events.
	 */
	@SuppressWarnings("unchecked")
	protected void doInterval() {

		// Take a snapshot of the current state of the dir for comparisons
		takeSnapshot();

		// Iterate through the map of current files and compare
		// them for differences etc...
		Iterator currentIt = currentFiles.keySet().iterator();

		while (currentIt.hasNext()) {
			String fileName = (String) currentIt.next();
			// Long lastModified = (Long) currentFiles.get(fileName);

			// If this file did not exist before, but it does now, then it's
			// been added
			//if (!prevFiles.containsKey(fileName)) {
				// DirectorySnapshot.addFile(fileName);
				resourceAdded(new File(fileName));
			//}
			// If this file did exist before
			// 修改升级包不做处理
			/*
			 * else if (prevFiles.containsKey(fileName)) { Long prevModified =
			 * (Long) prevFiles.get(fileName); // If this file existed before
			 * and has been modified if (prevModified.compareTo(lastModified) !=
			 * 0) { resourceChanged(new File(fileName)); } }
			 */
		}

		// Now we need to iterate through the list of previous files and
		// see if any that existed before don't exist anymore
		// 删除升级包不做处理
		/*
		 * Iterator prevIt = prevFiles.keySet().iterator(); while
		 * (prevIt.hasNext()) { String fileName = (String) prevIt.next(); // If
		 * this file did exist before, but it does not now, then // it's been
		 * deleted if (!currentFiles.containsKey(fileName)) {
		 * resourceDeleted(fileName); } }
		 */
	}

	public UnPackParser getUnPackParser() {
		return unPackParser;
	}

	public void setUnPackParser(UnPackParser unPackParser) {
		this.unPackParser = unPackParser;
	}

	/**
	 * For testing only.
	 * 
	 * @param args
	 */
	// public static void main(String[] args) {
	// DirectoryWatcher dw = new DirectoryWatcher(
	// "D:/test/upgrade/temp/pack/", 5);
	// dw.addListener(new FileListener());
	// dw.start();
	// }
}
