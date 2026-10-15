// Builds resources/lost/patches/minecraft_1_li02.jar, which EBALIA puts before the game on the class path.
//
// The Create New World screen of Minecraft 1.Li02 (class vt, Minecraft 1.1's GuiCreateWorld) never gives the game its
// player controller, so creating a world always ended in a NullPointerException in Minecraft.e. This puts back what
// Minecraft 1.1 does there: the Creative game mode gets the creative controller (and game type 1), every other mode
// the survival one. The rest of the screen is left as the version has it (the "Kill" world, the stopped sounds).
//
// With Java 8 and ASM 9 (the jars are in the version's own bin/org/ow2/asm folder):
//   javac -cp asm-9.2.jar:asm-tree-9.2.jar Li02CreateWorld.java
//   java -cp asm-9.2.jar:asm-tree-9.2.jar:<bin/minecraft.jar>:. Li02CreateWorld <bin/minecraft.jar> minecraft_1_li02.jar
import java.io.ByteArrayOutputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipOutputStream;
import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassWriter;
import org.objectweb.asm.Opcodes;
import org.objectweb.asm.tree.*;

public class Li02CreateWorld {
    public static void main(String[] args) throws Exception {
        byte[] original;
        try (ZipFile jar = new ZipFile(args[0]); InputStream in = jar.getInputStream(jar.getEntry("vt.class"))) {
            ByteArrayOutputStream bytes = new ByteArrayOutputStream();
            byte[] buffer = new byte[8192];
            for (int n; (n = in.read(buffer)) > 0; ) bytes.write(buffer, 0, n);
            original = bytes.toByteArray();
        }
        ClassNode screen = new ClassNode();
        new ClassReader(original).accept(screen, 0);
        MethodNode pressed = null;
        for (MethodNode m : screen.methods) if (m.name.equals("a") && m.desc.equals("(Lzr;)V")) pressed = m;
        if (pressed == null) throw new IllegalStateException("vt.a(zr) not found");
        // The game's field of the screen (GuiScreen.mc), as the method reads it.
        FieldInsnNode game = null;
        for (AbstractInsnNode i : pressed.instructions.toArray())
            if (i instanceof FieldInsnNode && ((FieldInsnNode) i).desc.equals("Lnet/minecraft/client/Minecraft;")) { game = (FieldInsnNode) i; break; }
        // gameMode.equals("creative") followed by an empty branch: IFEQ to the instruction right after it.
        JumpInsnNode empty = null;
        for (AbstractInsnNode i : pressed.instructions.toArray()) {
            if (!(i instanceof LdcInsnNode) || !"creative".equals(((LdcInsnNode) i).cst)) continue;
            AbstractInsnNode equals = i.getNext(), jump = equals.getNext();
            if (jump instanceof JumpInsnNode && jump.getOpcode() == Opcodes.IFEQ && ((JumpInsnNode) jump).label == jump.getNext()) empty = (JumpInsnNode) jump;
        }
        if (game == null || empty == null) throw new IllegalStateException("The Create New World code is not the expected one");
        LabelNode after = empty.label, survival = new LabelNode();
        InsnList fix = new InsnList();
        fix.add(new InsnNode(Opcodes.ICONST_1));
        fix.add(new VarInsnNode(Opcodes.ISTORE, 5)); // game type 1: creative
        controller(fix, game, "ade");
        fix.add(new JumpInsnNode(Opcodes.GOTO, after));
        fix.add(survival);
        controller(fix, game, "acq");
        empty.label = survival;
        pressed.instructions.insert(empty, fix);
        ClassWriter writer = new ClassWriter(ClassWriter.COMPUTE_MAXS | ClassWriter.COMPUTE_FRAMES);
        screen.accept(writer);
        try (ZipOutputStream out = new ZipOutputStream(new FileOutputStream(args[1]))) {
            ZipEntry entry = new ZipEntry("vt.class");
            entry.setTime(315532800000L); // fixed date: the same input always gives the same file
            out.putNextEntry(entry);
            out.write(writer.toByteArray());
            out.closeEntry();
        }
    }
    // this.mc.playerController = new <type>(this.mc);
    private static void controller(InsnList code, FieldInsnNode game, String type) {
        code.add(new VarInsnNode(Opcodes.ALOAD, 0));
        code.add(new FieldInsnNode(Opcodes.GETFIELD, game.owner, game.name, game.desc));
        code.add(new TypeInsnNode(Opcodes.NEW, type));
        code.add(new InsnNode(Opcodes.DUP));
        code.add(new VarInsnNode(Opcodes.ALOAD, 0));
        code.add(new FieldInsnNode(Opcodes.GETFIELD, game.owner, game.name, game.desc));
        code.add(new MethodInsnNode(Opcodes.INVOKESPECIAL, type, "<init>", "(Lnet/minecraft/client/Minecraft;)V", false));
        code.add(new FieldInsnNode(Opcodes.PUTFIELD, "net/minecraft/client/Minecraft", "c", "Ljs;"));
    }
}
