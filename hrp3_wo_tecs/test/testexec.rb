#!/usr/bin/env ruby
# -*- coding: utf-8 -*-
#
#  TOPPERS Software
#      Toyohashi Open Platform for Embedded Real-Time Systems
# 
#  Copyright (C) 2016-2020 by Embedded and Real-Time Systems Laboratory
#              Graduate School of Information Science, Nagoya Univ., JAPAN
# 
#  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
#  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
#  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
#  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
#      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
#      スコード中に含まれていること．
#  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
#      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
#      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
#      の無保証規定を掲載すること．
#  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
#      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
#      と．
#    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
#        作権表示，この利用条件および下記の無保証規定を掲載すること．
#    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
#        報告すること．
#  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
#      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
#      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
#      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
#      免責すること．
# 
#  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
#  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
#  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
#  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
#  の責任を負わない．
# 
#  $Id$
# 

# 【実行方法】
#	testexec <処理内容> <処理対象>
#
#	処理内容：
#		デフォルト		buildとexec
#		build			ビルドのみ
#		exec			実行のみ
#		clean			クリーン処理
#
#	処理対象：
#		デフォルト		kernelとall
#		kernel			デフォルトとディレクトリが作られているカーネル
#		kernel<数字>	指定したビルドオプションのカーネル
#		all				ディレクトリが作られているすべてのテストプログラム
#		<テスト名>		指定したテストプログラム
#
# 【ターゲット毎のビルドオプション】
#	ターゲット毎のビルドオプションを，TARGET_OPTIONSに作成する．異なる
#	テスト用のビルドオプションを，各行に記述する．
#
#	各行（最初の行が0）に記述するビルドオプション：
#		0		一般的なテストプログラム
#		1		性能評価プログラム
#		2		タイマドライバシミュレータを用いたテストプログラム
#		3		FPUを使用するテストプログラム（ARM向け）
#
# 【ターゲット毎の実行方法】
#	ターゲット上でテストプログラムを実行するための記述を，TARGET_RUNに
#	作成する．

Encoding.default_external = 'utf-8'
require "pp"

#
#  テストプログラム毎に必要なオプションの定義
#
TEST_SPEC = {
  # 機能テストプログラム
  "calsvc"   => { SRC: "test_calsvc" },
  "cpuexc1"  => { SRC: "test_cpuexc1" },
  "cpuexc2"  => { SRC: "test_cpuexc2" },
  "cpuexc3"  => { SRC: "test_cpuexc3" },
  "cpuexc4"  => { SRC: "test_cpuexc4" },
  "cpuexc5"  => { SRC: "test_cpuexc5" },
  "cpuexc6"  => { SRC: "test_cpuexc6" },
  "cpuexc7"  => { SRC: "test_cpuexc7" },
  "cpuexc8"  => { SRC: "test_cpuexc8" },
  "cpuexc9"  => { SRC: "test_cpuexc9" },
  "cpuexc10" => { SRC: "test_cpuexc10" },
  "dlynse"   => { SRC: "test_dlynse" },
  "dtq1"     => { SRC: "test_dtq1" },
  "extsvc1"  => { SRC: "test_extsvc1" },
  "exttsk"   => { SRC: "test_exttsk", CDL: "test_pf_bitkernel" },
  "flg1"     => { SRC: "test_flg1" },
  "hrt1"     => { SRC: "test_hrt1" },
  "int1"     => { SRC: "test_int1" },
  "messagebuf1" => { SRC: "test_messagebuf1", CDL: "test_pf_bitkernel" },
  "messagebuf2" => { SRC: "test_messagebuf2", CDL: "test_pf_bitkernel" },
  "mprot1"   => { SRC: "test_mprot1" },
  "mprot2"   => { SRC: "test_mprot2" },
  "mutex1"   => { SRC: "test_mutex1", CDL: "test_pf_bitkernel" },
  "mutex2"   => { SRC: "test_mutex2", CDL: "test_pf_bitkernel" },
  "mutex3"   => { SRC: "test_mutex3", CDL: "test_pf_bitkernel" },
  "mutex4"   => { SRC: "test_mutex4", CDL: "test_pf_bitkernel" },
  "mutex5"   => { SRC: "test_mutex5", CDL: "test_pf_bitkernel" },
  "mutex6"   => { SRC: "test_mutex6", CDL: "test_pf_bitkernel" },
  "mutex7"   => { SRC: "test_mutex7", CDL: "test_pf_bitkernel" },
  "mutex8"   => { SRC: "test_mutex8", CDL: "test_pf_bitkernel" },
  "notify1"  => { SRC: "test_notify1" },
  "raster1"  => { SRC: "test_raster1", CDL: "test_pf_bitkernel" },
  "raster2"  => { SRC: "test_raster2" },
  "sched1"   => { SRC: "test_sched1" },
  "sem1"     => { SRC: "test_sem1" },
  "sem2"     => { SRC: "test_sem2" },
  "suspend1" => { SRC: "test_suspend1" },
  "sysman1"  => { SRC: "test_sysman1" },
  "sysman2"  => { SRC: "test_sysman2" },
  "sysstat1" => { SRC: "test_sysstat1" },
  "sysstat2" => { SRC: "test_sysstat2" },
  "task1"    => { SRC: "test_task1", CDL: "test_pf_bitkernel" },
  "tmevt1"   => { SRC: "test_tmevt1" },
  "tprot1"   => { SRC: "test_tprot1", CDL: "test_pf_bitkernel" },
  "tprot2"   => { SRC: "test_tprot2", CDL: "test_pf_bitkernel" },
  "tprot3"   => { SRC: "test_tprot3", CDL: "test_pf_bitkernel" },
  "tprot4"   => { SRC: "test_tprot4", CDL: "test_pf_bitkernel" },
  "tprot5"   => { SRC: "test_tprot5", CDL: "test_pf_bitkernel" },
  "twdnfy1"  => { SRC: "test_twdnfy1" },
  "twdnfy2"  => { SRC: "test_twdnfy2" },

  # オーバランハンドラ機能拡張パッケージの機能テストプログラム
  "ovrhdr1"  => { SRC: "test_ovrhdr1" },
  "ovrhdr2"  => { SRC: "test_ovrhdr2" },
  "ovrhdr3"  => { TARGET: 2, SRC: "simt_ovrhdr3", DEFS: "-DHRT_CONFIG1" },
  "ovrhdr4"  => { SRC: "test_ovrhdr4" },
  "ovrhdr5"  => { SRC: "test_ovrhdr5" },

  # 制約タスク拡張パッケージの機能テストプログラム
  "rstr1"    => { SRC: "test_rstr1" },
  "rstr2"    => { SRC: "test_rstr2" },

  # サブ優先度機能拡張パッケージの機能テストプログラム
  "subprio1" => { SRC: "test_subprio1" },
  "subprio2" => { SRC: "test_subprio2" },

  # システム時刻管理機能テストプログラム
  "systim1" => { TARGET: 2, SRC: "simt_systim1",
								DEFS: "-DHRT_CONFIG1 -DHOOK_HRT_EVENT" },
  "systim2" => { TARGET: 2, SRC: "simt_systim2",
								DEFS: "-DHRT_CONFIG1 -DHOOK_HRT_EVENT" },
  "systim3" => { TARGET: 2, SRC: "simt_systim3",
								DEFS: "-DHRT_CONFIG1 -DHOOK_HRT_EVENT" },
  "systim4" => { TARGET: 2, SRC: "simt_systim4",
								DEFS: "-DHRT_CONFIG2 -DHOOK_HRT_EVENT" },
  "systim1_64hrt" => { TARGET: 2, SRC: "simt_systim1_64hrt",
								DEFS: "-DHRT_CONFIG3 -DHOOK_HRT_EVENT" },
  "systim2_64hrt" => { TARGET: 2, SRC: "simt_systim2_64hrt",
								DEFS: "-DHRT_CONFIG3 -DHOOK_HRT_EVENT" },
  "systim3_64hrt" => { TARGET: 2, SRC: "simt_systim3_64hrt",
								DEFS: "-DHRT_CONFIG3 -DHOOK_HRT_EVENT" },

  # タイムウィンドウ管理機能のテストプログラム
  "twd1" => { TARGET: 2, SRC: "simt_twd1", DEFS: "-DHRT_CONFIG1" },

  # ドリフト調整機能拡張パッケージのシステム時刻管理機能テストプログラム
  "drift1"        => { TARGET: 2, SRC: "simt_drift1",
								DEFS: "-DHRT_CONFIG1 -DHOOK_HRT_EVENT" },
  "drift1_64hrt"  => { TARGET: 2, SRC: "simt_drift1_64hrt",
								DEFS: "-DHRT_CONFIG3 -DHOOK_HRT_EVENT" },
  "drift1_64ops"  => { TARGET: 2, SRC: "simt_drift1",
				DEFS: "-DHRT_CONFIG1 -DHOOK_HRT_EVENT -DUSE_64BIT_OPS" },
  "systim1_64ops" => { TARGET: 2, SRC: "simt_systim1",
				DEFS: "-DHRT_CONFIG1 -DHOOK_HRT_EVENT -DUSE_64BIT_OPS" },
  "systim2_64ops" => { TARGET: 2, SRC: "simt_systim2",
				DEFS: "-DHRT_CONFIG1 -DHOOK_HRT_EVENT -DUSE_64BIT_OPS" },
  "systim3_64ops" => { TARGET: 2, SRC: "simt_systim3",
				DEFS: "-DHRT_CONFIG1 -DHOOK_HRT_EVENT -DUSE_64BIT_OPS" },

  # 性能評価プログラム
  "measure1n" => { TARGET: 1, SRC: "perf_measure1", CDL: "perf_pf" },
  "measure1" => { TARGET: 1, SRC: "perf_measure1", CDL: "perf_pf",
										NK_DEFS: "-DHIST_INVALIDATE_CACHE" },
  "pcalsvc1" => { TARGET: 1, SRC: "perf_pcalsvc1", CDL: "perf_pf",
										NK_DEFS: "-DHIST_INVALIDATE_CACHE" },
  "pextsvc1" => { TARGET: 1, SRC: "perf_pextsvc1", CDL: "perf_pf",
										NK_DEFS: "-DHIST_INVALIDATE_CACHE" },

  # 性能評価プログラム
  "perf0" => { TARGET: 1, CDL: "perf_pf", NK_DEFS: "-DHIST_INVALIDATE_CACHE" },
  "perf1" => { TARGET: 1, CDL: "perf_pf", NK_DEFS: "-DHIST_INVALIDATE_CACHE" },
  "perf2" => { TARGET: 1, CDL: "perf_pf", NK_DEFS: "-DHIST_INVALIDATE_CACHE" },
  "perf3" => { TARGET: 1, CDL: "perf_pf", NK_DEFS: "-DHIST_INVALIDATE_CACHE" },
  "perf4" => { TARGET: 1, CDL: "perf_pf", NK_DEFS: "-DHIST_INVALIDATE_CACHE" },
  "perf5" => { TARGET: 1, CDL: "perf_pf", NK_DEFS: "-DHIST_INVALIDATE_CACHE" },

  "perf0u" => { TARGET: 1, SRC: "perf0", CFG: "perf0u", CDL: "perf_pf",
				NK_DEFS: "-DHIST_INVALIDATE_CACHE" },
  "perf1u" => { TARGET: 1, SRC: "perf1", CFG: "perf1u", CDL: "perf_pf",
				NK_DEFS: "-DHIST_INVALIDATE_CACHE" },

  # ライブラリのテスト
  "prbstr"  => { SRC: "test_prbstr" },

  # ARM向けテストプログラム
  "arm_cpuexc1" => { SRC: "arm_cpuexc1", SRCDIR: "arch/arm_gcc/test" },
  "arm_cpuexc2" => { SRC: "arm_cpuexc2", SRCDIR: "arch/arm_gcc/test" },
  "arm_fpu1" => { TARGET: 3, SRC: "arm_fpu1", SRCDIR: "arch/arm_gcc/test" },
}

#
#  カーネルライブラリの作成
#
def BuildKernel(target, mkdirFlag=false)
  return unless $targetOptions.has_key?(target)

  kernelDir = "KERNELLIB" + target.to_s
  if !Dir.exist?(kernelDir)
    if mkdirFlag
      Dir.mkdir(kernelDir)
    else
      return
    end
  end

  Dir.chdir(kernelDir) do
    puts("== building: #{kernelDir} ==")
    configCommand = "ruby #{$usedSrcDir}/configure.rb"
    configCommand += " #{$targetOptions[target]}"
    configCommand += " -f"
    puts(configCommand)
    system(configCommand)
    system("make libkernel.a")
    if File.exist?("Makefile.bak")
      File.delete("Makefile.bak")
    end
  end
end

#
#  全カーネルライブラリの作成
#
def BuildAllKernel()
  $targetOptions.keys.each do |target|
    BuildKernel(target)
  end
end

#
#  テストプログラムの作成
#
def BuildTest(test, testSpec, mkdirFlag=false)
  testName = test.tr("a-z", "A-Z")
  objDir = "OBJ-#{testName}"

  if !Dir.exist?(objDir)
    if mkdirFlag
      Dir.mkdir(objDir)
    else
      return
    end
  end

  Dir.chdir(objDir) do
    puts("== building: #{testName} ==")
    configCommand = "ruby #{$usedSrcDir}/configure.rb"
    if testSpec.has_key?(:TARGET)
      configCommand += " #{$targetOptions[testSpec[:TARGET]]}"
    else
      configCommand += " #{$targetOptions[0]}"
    end
    if testSpec.has_key?(:SRCDIR)
      configCommand += " -a \"#{$usedSrcDir}/#{testSpec[:SRCDIR]}" \
											" #{$usedSrcDir}/test\""
    else
      configCommand += " -a #{$usedSrcDir}/test"
    end

    if !testSpec.has_key?(:DEFS)
      if (testSpec.has_key?(:TARGET))
        kernelDir = "KERNELLIB" + testSpec[:TARGET].to_s
      else
        kernelDir = "KERNELLIB0"
      end
      if Dir.exist?("../" + kernelDir)
        configCommand += " -L ../" + kernelDir
      end
    end
    if testSpec.has_key?(:SRC)
      configCommand += " -A #{testSpec[:SRC]}"
    else
      configCommand += " -A #{test}"
    end
    if testSpec.has_key?(:CFG)
      configCommand += " -c #{testSpec[:CFG]}.cfg"
    end
    if testSpec.has_key?(:CDL)
      configCommand += " -C #{testSpec[:CDL]}.cdl"
    else
      configCommand += " -C test_pf.cdl"
    end
    if testSpec.has_key?(:SYSOBJ)
      configCommand += " -S \"" \
			+ testSpec[:SYSOBJ].split(/\s+/).map{|f| f+".o"}.join(" ") \
			+ "\""
    end
    if testSpec.has_key?(:APPLOBJ)
      configCommand += " -U \"" \
			+ testSpec[:APPLOBJ].split(/\s+/).map{|f| f+".o"}.join(" ") \
			+ "\""
    end
    if testSpec.has_key?(:OPTS)
      configCommand += " -o \"#{testSpec[:OPTS]}\""
    end
    if testSpec.has_key?(:DEFS)
      configCommand += " -O \"#{testSpec[:DEFS]}\""
    end
    if testSpec.has_key?(:NK_DEFS)
      configCommand += " -O \"#{testSpec[:NK_DEFS]}\""
    end
    puts(configCommand)
    system(configCommand)
    system("make")
    if File.exist?("Makefile.bak")
      File.delete("Makefile.bak")
    end
  end
end

#
#  全テストプログラムの作成
#
def BuildAllTest()
  TEST_SPEC.each do |test, testSpec|
    BuildTest(test, testSpec)
  end
end

#
#  テストプログラムの実行
#
def ExecTest(test, testSpec)
  testName = test.tr("a-z", "A-Z")
  objDir = "OBJ-#{testName}"

  return unless Dir.exist?(objDir)

  Dir.chdir(objDir) do
    puts("== executing: #{testName} ==")
    if File.exist?("../TARGET_RUN")
      system(`cat ../TARGET_RUN`)
    else
      system("./asp")
    end
  end
end

#
#  全テストプログラムの実行
#
def ExecAllTest()
  TEST_SPEC.each do |test, testSpec|
    ExecTest(test, testSpec)
  end
end

#
#  カーネルライブラリのクリーン
#
def CleanKernel(target)
  return unless $targetOptions.has_key?(target)

  kernelDir = "KERNELLIB" + target.to_s
  if Dir.exist?(kernelDir)
    Dir.chdir(kernelDir) do
      system("make clean")
    end
  end
end

#
#  全カーネルライブラリのクリーン
#
def CleanAllKernel()
  $targetOptions.keys.each do |target|
    CleanKernel(target)
  end
end

#
#  テストプログラムのクリーン
#
def CleanTest(test, testSpec)
  testName = test.tr("a-z", "A-Z")
  objDir = "OBJ-#{testName}"

  return unless Dir.exist?(objDir)

  Dir.chdir(objDir) do
    system("make clean")
  end
end

#
#  全テストプログラムのクリーン
#
def CleanAllTest()
  TEST_SPEC.each do |test, testSpec|
    CleanTest(test, testSpec)
  end
end

#
#  ソースディレクトリ名を取り出す
#
if /^(.*)\/test\/testexec/ =~ $0
  $srcDir = $1
else
  $srcDir = "."
end

if /^\// =~ $srcDir
  $usedSrcDir = $srcDir
else
  $usedSrcDir = "../" + $srcDir
end

#
#  ターゲット依存のオプションを読む
#
$targetOptions = {}
File.open("TARGET_OPTIONS") do |file|
  file.each_line.with_index do |line, index|
    line.chomp!
    if line != ""
      $targetOptions[index] = line
    end
  end
end

#
#  パラメータで指定された処理の実行
#
$build_only = false
$exec_only = false
$clean_flag = false
$proc_flag = false

ARGV.each do |param|
  case param
  when "build"
    $build_only = true
  when "exec"
    $exec_only = true
  when "clean"
    $clean_flag = true

  when "kernel"
    if ($clean_flag)
      CleanAllKernel()
    else
      BuildAllKernel() unless $exec_only
      # カーネルには，execはない
    end
    $proc_flag = true

  when /^kernel([0-9]+)$/
    target = $1.to_i
    if ($clean_flag)
      CleanKernel(target)
    else
      BuildKernel(target, true) unless $exec_only
      # カーネルには，execはない
    end
    $proc_flag = true

  when "all"
    if ($clean_flag)
      CleanAllTest()
    else
      BuildAllTest() unless $exec_only
      ExecAllTest() unless $build_only
    end
    $proc_flag = true

  else
    if TEST_SPEC.has_key?(param)
      if ($clean_flag)
        CleanTest(param, TEST_SPEC[param])
      else
        BuildTest(param, TEST_SPEC[param], true) unless $exec_only
        ExecTest(param, TEST_SPEC[param]) unless $build_only
      end
    else
      puts("invalid parameter: #{param}")
    end
    $proc_flag = true
  end
end
if !$proc_flag
  # デフォルトの処理対象（kernelとall）
  if ($clean_flag)
    CleanAllKernel()
    CleanAllTest()
  else
    BuildAllKernel() unless $exec_only
    BuildAllTest() unless $exec_only
    ExecAllTest() unless $build_only
  end
end
