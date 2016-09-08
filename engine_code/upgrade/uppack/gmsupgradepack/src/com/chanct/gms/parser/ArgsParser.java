package com.chanct.gms.parser;

import java.io.File;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.GnuParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

import com.chanct.gms.config.Global;
import com.chanct.gms.utiles.Utiles;

/**
 * 
 * @ClassName: AgrsParser
 * @Description: 命令行参数解析器
 * @author shf
 * @date Dec 5, 2013 2:13:57 PM
 * 
 */
public class ArgsParser {
    /**
     * 
     * @Title: getOpts
     * @Description: 打包程序参数设置及解析
     * @param
     * @param args
     * @param
     * @return
     * @return boolean 返回类型
     * @throws
     */
    public boolean getOpts(String[] args) {

        OptionBuilder.withArgName("Update config file dir");
        OptionBuilder.hasArg();
        OptionBuilder.withDescription("升级配置文件绝对路径");
        Option opConfig = OptionBuilder.create("c");

        OptionBuilder.withArgName("Target dir");
        OptionBuilder.hasArg();
        OptionBuilder.withDescription("升级包存放绝对路径");
        Option opTarget = OptionBuilder.create("t");

        OptionBuilder.withArgName("h");
        OptionBuilder.hasArg(false);
        OptionBuilder.withDescription("命令行参数帮助");
        Option opHelp = OptionBuilder.create("h");

        OptionBuilder.withArgName("help");
        OptionBuilder.hasArg(false);
        OptionBuilder.withDescription("命令行参数帮助");
        Option opHelp2 = OptionBuilder.create("help");

        Options options = new Options();
        options.addOption(opConfig);
        options.addOption(opTarget);
        options.addOption(opHelp);
        options.addOption(opHelp2);

        CommandLineParser parser = new GnuParser();
        CommandLine cmd = null;
        try {
            cmd = parser.parse(options, args);
        } catch (ParseException exp) {
            System.err.println(new StringBuilder().append("参数解析错误.  Reason: ").append(exp.getMessage()).toString());
            return false;
        }

        if ((cmd.hasOption("help")) || (cmd.hasOption("h"))) {
            HelpFormatter formatter = new HelpFormatter();
            formatter.printHelp(" ", options);
            return false;
        }

        if (cmd.hasOption("c")) {
            if (Utiles.isFile(cmd.getOptionValue("c"))) {
                Global.CONFIG_FILE = cmd.getOptionValue("c");
                Global.CONFIG_FILE = Utiles.dealDirSeparate(Global.CONFIG_FILE);
                if (!new File(cmd.getOptionValue("c")).getName().equals(Global.UPGRADE_FILE_NAME)) {
                    System.out.println("参数错误.  Reason：Option '-c' must be '" + Global.UPGRADE_FILE_NAME + "'");
                    return false;
                }
            } else {
                System.out.println("解析参数错误.  Reason：Option '-c' must be upgrade config file");
                return false;
            }
        } else {
            System.out.println("Option '-c' is needed! Try '-help' or '-help' for more information.");

            return false;
        }

        if (cmd.hasOption("t")) {
            if (Utiles.isDirectory(cmd.getOptionValue("t"))) {
                Global.TARGET_FILE = cmd.getOptionValue("t");
                Global.TARGET_FILE = Utiles.dealDirSeparate(Global.TARGET_FILE);
                Global.TARGET_FILE = Utiles.dealDirectoryTail(Global.TARGET_FILE);
            } else {
                System.out.println("解析参数错误.  Reason：Option '-t' must be an exits directory");
                return false;
            }

        } else {
            System.out.println("Option '-t' is needed! Try '-h' or '-help' for more information.");
            return false;
        }

        return true;
    }
}
